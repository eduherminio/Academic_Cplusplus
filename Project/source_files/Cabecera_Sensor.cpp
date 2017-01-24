/* Author:      Eduardo Cáceres de la Calle
 * Subject:     Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */
 

#include "../header_files/Cabecera_Sensor.hpp"

                                // Definición de variables globales
float temp_global=100000;           // Temperatura generada. Inicializada a un valor arbitrario, de fácil detección para Control o Memoria
int DEF_FRECUENCIA=1;               // Frecuencia de generación de datos por defecto


/* Nombre:      Solicitud_Permiso
 * Tipo:        void
 * Función:     Solicita el alta a Control
 * Descripción:
 *              1.  Espera en s_soliciutd_permiso mientras se está escribiendo en shmemo_alta. De esta manera pide 'turno'
 *              2.  Espera en s_procesos_libres a que quede un hueco libre en Control  (controlado por s_procesos_libres)
 *              2.  Una vez s_procesos_libres deja paso, escribe su pid en la memoria compartida
 *              3.  Tras esto, permite a Control leer de esa memoria compartida (s_concesion_permiso)
 */
void Solicitud_Permiso()
{
    semaforo s_concesion_permiso("s1");     // IPCs con void Permiso_Sensor(), en Control
    semaforo s_solicitud_permiso("s2");
    semaforo s_procesos_libres("s3");       // IPC con Recepcion_info_sensor(), en Control

    memocomp permiso("shmemo_alta", OPEN, RDWR, sizeof(int));
    int *pid;
    pid= (int *)permiso.getPointer();

        cout<<"Esperando mi turno para pedir hueco"<<endl;
    s_solicitud_permiso.down(); // Permite que un solo sensor pueda entrar a la vez
        cout<<"Esperando hueco"<<endl;
    s_procesos_libres.down();   // Siempre y cuando quede hueco para él, claro
    *pid=getpid();      // Solo entonces escribe en la memoria compartida

        cout<<"pid "<<getpid()<<" mandado"<<endl;
    s_concesion_permiso.up();   // Una vez ha escrito, indica a Control que ya puede leer
}


/* Nombre:      Generacion_temperatura
 * Tipo:        void
 * Función:     Genera temperaturas pseudo-aleatorias, cuyo valor en nuestr caso está entre 20 y 30
 * Argumentos:
 *              @ freq              frecuencia con la que se generan las temperaturas
 *Otros:
 *              - Se generan temperaturas con dos decimales.
 *              - La generación pseudo-aleatoria está basada en el tiempo actual y en el propio pid del proceso
 *              - Es controlable la frecuencia de generación de temperaturas a través del paso al main() de su valor
 */
void Generacion_temperatura(int argc, char *argv[])
{
    int freq=DEF_FRECUENCIA;              // Frecuencia por defecto en caso de que no se pase como parámetro

    if(argc==2)
        freq=atoi(argv[1]);

    while(true)
    {
        sleep(freq);                           // freq con la que los sensores generan temperaturas pseudoaleatorias

        time_t t_actual= time(&t_actual);
        srand(t_actual*getpid());
        float aux;
        aux= rand()%1001;           // números pseudo aleatorios entre 1 y 1000
        temp_global=(aux/100)+20;   // Desplazamiento de la coma (entre 1 y 10) y suma de 20

        cout<<"Temp. aleat= "<<showpos<<temp_global<<noshowpos<<endl;   // Muestra el signo,  aunque no se necesario en el rango elegido
    }
}


/* Nombre:      Envio_info_contorl                 (función de tratamiento de la señal SIGALRM)
 * Tipo:        void
 * Argumentos:
 *              @ sig    Señal recibida
 * Función:     Capta una señal de envío de información, y obedece
 * Descripción:
 *              Espera en s_envio_info permiso para escribir en la memoria (cuando se haya leído el paquete enviado)
 *
 */
void Envio_info_control(int sig)    // Función de tratamiento de SIGUSR1
{
    cout<<"\nSolicitud de informacion recibida"<<endl;

    // APERTURA DE IPC'S
    semaforo s_envio_info("s4");
    semaforo s_recepcion_info("s5");
    memocomp informacion("shmemo_info", OPEN, RDWR, 30*sizeof(char));
        char *paquete_info;
        paquete_info= (char *)informacion.getPointer();

    s_envio_info.down();
        cout<<"Escritura de informacion"<<endl;

    string cadena= Generacion_paquete_info();

    strcpy(paquete_info, cadena.c_str());

    cout<<"Mandamos: "<<paquete_info<<endl;
     s_recepcion_info.up();
}


/* Nombre:      Generacion_paquete_info
 * Tipo:        void
 * Función:     Junta toda la información que hay que enviar en una cadena de caracteres tipo string
*/
string Generacion_paquete_info()
{
    char c1[6], c2[6], c3[10];
    sprintf(c1,"%2.2f",temp_global);                        // Uso de | como primer separador

    sprintf(c2,"%d",getpid());                    // Uso de * como segundo separador

    time_t t_actual=time(&t_actual);
    sprintf(c3, "%lld", (long long) t_actual);

    string s1=c1;
    string s2=c2;
    string s3=c3;

    string cadena_local= s1+ "|" + s2 +"*" + s3;

    return cadena_local;
}

/* Nombre:      ctrl_c                  (función de tratamiento de la señal SIGINT)
 * Tipo:        void
 * Argumentos:
 *              @ sig    Señal recibida
 * Función:     Capta una señal de terminación, cerra los IPC abiertos y finaliza la ejecución
*/
void ctrl_c(int sig)
{
    cout<<"ctrl+c recibido\nCerrando IPCs"<<endl;

    semaforo s_concesion_permiso("s1");         //Re-apertura previa para que sean reconocidos dentro de esta función
        s_concesion_permiso.close();
    semaforo s_solicitud_permiso("s2");
        s_solicitud_permiso.close();
    semaforo s_procesos_libres("s3");
        s_procesos_libres.close();

    memocomp permiso("shmemo_alta", OPEN, RDWR, sizeof(int));
        permiso.cerrar();

    // Después del resto, por si no han llegado a crearse
    semaforo s_envio_info("s4");
        s_envio_info.close();
    semaforo s_recepcion_info("s5");
        s_recepcion_info.close();

    memocomp informacion("shmemo_info", OPEN, RDWR, sizeof(time_t));
        informacion.cerrar();

    cout<<"Proceso "<<getpid()<<" finalizado correctamente"<<endl;
    exit(0);
}

