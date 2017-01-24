/* Author:      Eduardo C�ceres de la Calle
 * Subject:     Inform�tica Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */

 
#include "../header_files/Cabecera_Control.hpp"

                                    // Definici�n de variables globales
vector<int> Sensores_activos;           // Vector que contiene los pids de los sensores conectados a Control
string cadena_global;                   // Cadena que contiene la informaci�n recibida de un sensor
bool act_alarm=0;                       // Variable que permite al hilo info_sensor reconocer  cu�ndo un sensor se ha ca�do
int PATH_MAX=100;                       // M�ximo tama�o de la cadena en la que se introducir� la ubicaci�n actual
int DEF_MAX_SENSORES=2;                 // Sensores m�ximos por defecto
int DEF_FRECUENCIA=2;                   // Frecuencia de lectura por defecto

/* Nombre:      Creacion_IPCs
 * Tipo:        void
 * Funci�n:     Agrupa la creacion de sem�foros, memorias compartidas y colas que se usar�n en control
 * Argumentos:
 *              @ MAX_SENSORES        n� m�ximo de sensores conectados simult�neamente
 */
void Creacion_IPCs(int MAX_SENSORES)
{
    semaforo s_concesion_permiso("s1", 0);                  // Permito leer a Permiso_Sensor() de shmemo_alta
    semaforo s_solicitud_permiso("s2", 1);                  // Indica a Solicitud_Permiso(), en Sensor, que puede escribir en shmemo_alta
    semaforo s_procesos_libres("s3", MAX_SENSORES);         // Permite realmente a Solicitud_Permiso() escribir en shmemo_alta su pid
    semaforo s_envio_info("s4", 1);                             // Permite a Envio_info_control(), Sensor, escribir en shmemo_info
    semaforo s_recepcion_info("s5", 0);                         // Permite a Recepcion_info_sensor() leer de shmemo_info
    semaforo s_envio_lista("slista", 0);                    // Indica a Envio_info_Lista() la existencia de una nueva cadena_global,
                                                                    // permiti�ndole su env�o a trav�s de colaunica
    semaforo s_toma_dato("stomadato", 1);                   // Permite a Informacion_Sensor() tomar un nuevo dato (reescribir en cadena_global)

    // Estos dos �ltimos, evitables con una cola de mensajes local que mande los paquetes de informaci�n que recibe a Envio_info_Lista.
    // Abstray�ndonos un paso m�s, esta cola puede re-dirigirse directamente a Lista (a -> b -> c = a -> c):
    //                                  Recepcion -> Envio_Lista -> Lista  ==  Recepcion -> Lista
    // Los sem�foros s1 y s2 son evitables si se usan colas de mensajes en vez de memoria compartida para gestionar el alta de sensores

    memocomp permiso("shmemo_alta", CREAT, RDWR, sizeof(int));
    memocomp informacion("shmemo_info", CREAT, RDWR, 30*sizeof(char));

    colamsg cola_unica("colaunica", CREAT, RDWR,30*sizeof(char));
}


/* Nombre:      Destruccion_IPCs
 * Tipo:        void
 * Funci�n:     Agrupa el cierre y destruccion de sem�foros, memorias compartidas y colas que se crearon en control
 */
void Destruccion_IPCs()
{
    cout<<"Envio de senyal de terminacion a los sensores activos..."<<endl;
    for(int i=0;i<Sensores_activos.size();i++)
        kill(Sensores_activos[i],SIGINT);


    sleep(1);           // Para dar tiempo a los sensores a cerrar los ipc, antes de destruirlos
    cout<<"Cerrando y destruyendo IPCs..."<<endl;

    semaforo s_concesion_permiso("s1");         // Re-apertura previa para que sean reconocidos dentro de esta funci�n
        s_concesion_permiso.close();
        s_concesion_permiso.unlink();
    semaforo s_solicitud_permiso("s2");
        s_solicitud_permiso.close();
        s_concesion_permiso.unlink();
    semaforo s_procesos_libres("s3");
        s_procesos_libres.close();
        s_procesos_libres.unlink();
    semaforo s_envio_info("s4");
        s_envio_info.close();
        s_envio_info.unlink();
    semaforo s_recepcion_info("s5");
        s_recepcion_info.close();
        s_recepcion_info.unlink();
    semaforo s_envio_lista("slista");
        s_envio_lista.close();
        s_envio_lista.unlink();
    semaforo s_toma_dato("stomadato");
        s_toma_dato.close();
        s_toma_dato.unlink();

    memocomp permiso("shmemo_alta", OPEN, RDWR, sizeof(int));
        permiso.cerrar();
        permiso.unlink();
    memocomp informacion("shmemo_info", OPEN, RDWR, sizeof(time_t));
        informacion.cerrar();
        informacion.unlink();

    colamsg cola_unica("colaunica", 0, RDWR);
        cola_unica.close();
        cola_unica.unlink();

    cout<<"Todo cerrado y destruido"<<endl;
}


/* Nombre:      Extraccion_argumentos
 * Tipo:        void
 * Funci�n:     Obtiene en n� m�ximo de sensores conectados simultaneamente y la frecuencia de lectura de datos
 * Argumentos:
 *              @ argc              n� de argumentos introducidos en el main()
 *              @ argv              argumentos del main()
 *              @ MAX_SENSORES      n� m�ximo de sensores conectados simult�neamente.   Por referencia
 *              @ frecuencia        frecuencia de lectura de datos de los sensores.     Por referencia
 * Otros:
 *              Toma por defecto MAX_SENSORES=2 y frecuencia=2 si no existe paso de par�metros
 *              Adicionalmente, muestra esta informaci�n por pantalla junto al nombre del archivo
 */
void Extraccion_argumentos(int argc, char *argv[], int &MAX_SENSORES, int &frecuencia)
{
    MAX_SENSORES=DEF_MAX_SENSORES;
    frecuencia=DEF_FRECUENCIA;

    if(argc==3)
        frecuencia=atoi(argv[2]);
    if(argc==2)
        MAX_SENSORES=atoi(argv[1]);         // Extracci�n de los argumentos necesarios

                                        // [Extra] Extracci�n del nombre del ejecutable
    char name_char[sizeof(argv[0])+1];          // Sin el +1, stack smashing detected (aborta y genera 'core')
    strcpy(name_char, argv[0]);

    if(strncmp(name_char, "./", 2)==0)  // Se asegura de eliminar el './' si ha sido usado para la ejecuci�n
    {
        char aux[sizeof(name_char)];
        strcpy(aux, name_char);
        for(int i=0;i<sizeof(name_char);i++)
            name_char[i]=aux[i+2];
    }

    cout<<'\n'<<"Nombre: "<<name_char<<endl;
    cout<<"Sensores admitidos: "<<MAX_SENSORES<<endl;
    cout<<"Frecuencia de lectura: cada "<<frecuencia<<" segundos"<<'\n'<<endl;
}


/* Nombre:      Extraccion_direccion
 * Tipo:        string
 * Funci�n:     Obtiene la direcci�n en la cual est� ubicada Lista (es decir, el 'working directory'
 */
string Extraccion_direccion()
{
    char* cwd;
    char buff[PATH_MAX + 1];

    cwd = getcwd(buff, PATH_MAX + 1);
    if( cwd != NULL )
    {
        cout<<"Lista se encuentra en: "<<cwd<<endl;

        return cwd;
    }

    cout<<"Error al hallar la direcci�n. Revise tama�o del buffer";
}


/* Nombre:      Permiso_Sensor              (�nica funci�n del prceso padre)
 * Tipo:        void
 * Funci�n:     Hilo encargado de gestionar las altas de sensores. Mantiene comunicaci�n con cada Sensor ejecutado
 * Descripci�n:
 *              1.  Espera en s_concesion_permiso mientras no se cumplan las siguientes condiciones (ambas)
 *                  -   No hay ning�n sensor escribiendo en la memoria compartida   (controlado por s_solicitud_permiso)
 *                  -   Existe alg�n sensor intentando entrar y hay hueco libre para ese sensor  (controlado por s_procesos_libres)
 *              2.  Una vez s_concesion_permiso deja paso, lee el pid del nuevo sensor desde la memoria compartida
 *                  y lo almacena en vector<int> Sensores_activos
 *              3.  Tras esto, permite a un sensor acceder a la memoria compartida (s_solicitud_permiso)
 */
void Permiso_Sensor()
{
    semaforo s_concesion_permiso("s1");         // IPCs con Solicitud_Permiso(), en Sensor
    semaforo s_solicitud_permiso("s2");

    memocomp permiso("shmemo_alta", OPEN, RDWR, sizeof(int));
    int *pid;
    pid= (int *)permiso.getPointer();

    cout<<"<alta> En Permiso_Sensor, esperando introduccion sensores"<<endl;

    while(true)
    {
        s_concesion_permiso.down();  // Lo subir�n una conjunci�n de s2 y s3 desde Sensor
            cout<<"<alta> Introduccion de un nuevo sensor, de pid: "<<*pid<<endl;
        Sensores_activos.push_back(*pid);
        s_solicitud_permiso.up();

        cout<<"<alta> Nuevos sensores activos: "<<endl;
        for(int i=0;i<Sensores_activos.size();i++)
            cout<<" "<<Sensores_activos[i];
    }
    cout<<"Permiso_Sensor finalizado"<<endl;
}


/* Nombre:      Envio_info_lista            (funci�n del thread hilo_lista)
 * Tipo:        void
 * Funci�n:     Hilo encargado de gestionar el env�o de informaci�n a Lista tras su lectura. Se comunica con Informacion_Sensor()
 * Descripci�n:
 *              1.  Espera en s_envio_lista a que haya nueva informaci�n que enviar
 *              2.  Una vez s_envio_lista deja paso, env�a todos los datos recibidos un Sensor, compactados en cadena_global
 *              3.  Tras esto, permite a Informacion_Sensor reescribir sobre cadena_global subiendo s_toma_dato
 */
void Envio_info_lista()
{
    semaforo s_envio_lista("slista");           // IPCs con Informacion_Sensor()
    semaforo s_toma_dato("stomadato");

    colamsg cola_unica("colaunica", OPEN, RDWR,30*sizeof(char));

    while(true)
    {
            cout<<"<hilo_lista> Esperando a que haya informacion que enviar"<<endl;
        s_envio_lista.down();   // Lo subir� el Informaci�n_Sensor cuando exista informaci�n nueva que mandar,
        cola_unica.send(cadena_global.c_str(), 30*sizeof(char));
        s_toma_dato.up();   // Permite reescribir sobre cadena_global
    }
    cout<<"Envio_info_lista finalizado"<<endl;
}


/* Nombre:      Informacion_Sensor          (funci�n del thread hilo_info)
 * Tipo:        void
 * Funci�n:     Hilo encargado de gestionar el env�o de informaci�n a Lista tras su lectura. Se comunica con Informacion_Sensor()
 * Argumentos:
 *              @ freq          frecuencia de lectura de datos de los sensores
 *              @ MAX_SENSORES  n� m�ximo de sensores conectados simultaneamente
 * Descripci�n:
 *              1.  Espera 'freq' segundos
 *              2.  Recorre los conectados, haciendo:
 *                  1. Espera al env�o de la informaci�n anterior de Envio_info_Lista() (s_toma_dato)
 *                  2. Llama a Recepcion_info_sensor()
 *                  3. Permite el env�o de informaci�n de Envio_info_Lista (s_toma_dato)
 */
void Informacion_Sensor(int freq, int MAX_SENSORES)
{
    semaforo s_envio_lista("slista");
    semaforo s_toma_dato("stomadato");

    while(true)
    {
        sleep(freq);        // Frecuencia con la que se piden los datos a los sensores

        int sensor_actual=0;
        while(sensor_actual<Sensores_activos.size())    // Iterador: sensor actual
        {
            cout<<"<info> Esperando permiso para tomar dato"<<endl;

            s_toma_dato.down();   // Lo sube el hilo de env�o cuando ya ha mandado cadena_global
                Recepcion_info_sensor(sensor_actual);
            s_envio_lista.up(); // Una vez copiada la informaci�n a cadena_global, deja enviarla

            sensor_actual++;
        }
    }
        cout<<"Informacion_Sensor finalizado"<<endl;
}


/* Nombre:      Reepcion_info_sensor
 * Tipo:        void
 * Funci�n:     Recibe la informaci�n de un sensor
 * Argumentos:
 *              @ sensor_acutual    posici�n del sensor del que se va a recibir info. en vector<int> Sensores_activos
 * Descripci�n:
 *              1.  Solicita, mediante la se�al SIGUSR1, informaci�n al sensor que ocupa la posici�n sensor_actual
 *              2.  Activa una alarma de tres segundos, tiempo que le concede al sensor para responder
 *              3.  Espera en s_recepcion_info
 *                  - Si el sensor no responde (act_alarm==1)
 *                      a. Llama a Reestructuracion_tras_alarma
 *                      b. indica a los sensores en espera que queda un hueco libre (s_procesos_libres)
 *                  - Si el sensor env�a informaci�n
 *                      a. Lectura de la cadena que contiene los datos de la cadena compartida
 *                      b. Copia de esa cadena a la variable global cadena_global, a la que podr� acceder Envio_info_Lista()
 *                      c. Desactiva la alarma
 *             4. En cualquier de los casos, da permiso a Envio_invo_control(), en Sensor , para escribir en shmemo_info
 */
void Recepcion_info_sensor(int sensor_actual)
{
    semaforo s_procesos_libres("s3");               //  ICPs con Solicitud_Permiso(), en sensor
    semaforo s_envio_info("s4");                    // IPCs con Envio_info_control(), en Sensor
    semaforo s_recepcion_info("s5");

    memocomp informacion("shmemo_info", OPEN, RDWR, 30*sizeof(char));

    cout<<"<info> Pidiendo info al sensor "<<sensor_actual<<": "<<Sensores_activos[sensor_actual]<<endl;
    kill(Sensores_activos[sensor_actual],SIGUSR1);

    alarm(3);

    act_alarm=0;                // Ejecuci�n parada al morir sensor.
    s_recepcion_info.down();    // Lo sube un sensor

    if(act_alarm==1)            // Si lo sube Alarma, al acabar:
    {
        Reestructuracion_tras_alarma(sensor_actual); // Llamada a la funci�n que realmente trata la se�al de alarma
        s_procesos_libres.up();    // Indica a Solicitud_Permiso(), en Sensor, que hay hueco para un sensor m�s

        cout<<"\n<info>Fin del tratamiento de la alarma.\nPuede(n) entrar "<<s_procesos_libres.get()<<" sensor(es)"<<endl;
    }

    else
    {
        char *paquete_info;
        paquete_info=(char *)informacion.getPointer();
        cadena_global= paquete_info;

        alarm(0);       // Desactivaci�n de la alarma una vez hayan llegado y est�n lista la info.
    }

    s_envio_info.up();  // Fuera del else: permite a otro sensor escribir en shmemo_info
}


/* Nombre:      Reestructuraci�n_tras_alarma
 * Tipo:        void
 * Funci�n:     Mata al proceso correspondiente al sensor fallido (timed out) y elimna ese sensor del vector de sensores activos
 * Argumentos:
 *              @ sensor_acutual    posici�n del sensor del que se va a recibir info. en vector<int> Sensores_activos
 * Descripci�n:
 *              1.  Env�a una se�al de terminaci�n inmediata al sensor que ocupa la posici�n sensor_actual en el vector Sensores_activos
 *              2.  Elimina el sensor fallido del vector de sensores activos
 * Otros:
 *              Se empela la se�al SIGKILL en lugar de SIGINT para matar el proceso Sensor que no responde
 *              para evitar que el Sensor pueda recibir SIGUSR1 antes de SIGINT y pueda prioridad a la primera
 *              Esto no ser�a necesario en caso de implementaci�n de proridades entre las se�ales (a ampliar)
 */
void Reestructuracion_tras_alarma(int sensor_actual)
{
    cout<<"\nSensor que ha fallado: "<<sensor_actual<<": "<<Sensores_activos[sensor_actual]<<endl;
    kill(Sensores_activos[sensor_actual],SIGKILL);

    int j=0;
    vector<int> v_aux;

    for(int i=0; i<Sensores_activos.size()-1; i++)      // Copia Sensores_activos (sensores activos) a v_aux, obviando sensor_actual
    {
        if(sensor_actual==i)                                    // El iterador ha llegado a la posici�n a eliminar
            j++;

        v_aux.push_back(Sensores_activos[j]);
        j++;
    }

    Sensores_activos.resize(0);                         // Mecanismo que nos permite sobreescribir Sensores_activos de manera segura

    cout<<"Sensores activos tras la retirada del anterior: ";
    for(int i=0; i<v_aux.size(); i++)
    {
        cout<<" "<<v_aux[i];
        Sensores_activos.push_back(v_aux[i]);
    }
    cout<<'\n';

    sleep(2);   // Espera innecesaria, simplemente facilita al usuario seguir el comportamiento del programa
}


/* Nombre:      Alarma                  (funci�n de tratamiento de la se�al SIGALRM)
 * Tipo:        void
 * Argumentos:
 *              @ sig    Se�al recibida
 * Funci�n:     Capta una se�al de alarma, desviando la ejecuci�n de Recepcion_info_sensor para su tratamiento
 * Otros:
 *              N�tese que el hecho de no llamar a Reestructuracion_tras_alarma() desde aqu� nos permite evitar
 *              la declaraci�n global de la variable sensor_actual, al poder pasarle esta variable desde Recepcion_info_sensor()
 */
void Alarma (int sig)       // Funci�n de tratamiento de SIGALRM
{
    cout<<"\nAlarma de proceso caido (timeout) recibida"<<endl;

    sleep(1);
    semaforo s_recepcion_info("s5");

    // Recepcion_info_sensor se queda presumiblemente en s4
    // Para reanudar su ejecuci�n, hay que subir ese sem�foro 'manualmente',
    // y luego desviar la ejecuci�n del hilo principal a trav�s de la siguiente variable global:
     act_alarm=1;

     s_recepcion_info.up(); //   En Recepcion_info_sensor, perteneciente al hilo de recepci�n de informaci�n
}


/* Nombre:      ctrl_c                  (funci�n de tratamiento de la se�al SIGINT)
 * Tipo:        void
 * Argumentos:
 *              @ sig    Se�al recibida
 * Funci�n:     Capta una se�al de terminaci�n, finalizando la ejecuci�n del programa
 */
void ctrl_c(int sig)        // Funci�n de tretamiento de SIGINT
{
    cout<<"ctrl+c recibido"<<endl;

    sleep(3);       // Da tiempo a terminar a lista

    Destruccion_IPCs();

    cout<<"\nControl cerrado satisfactoriamente"<<endl;
    exit(0);
}

