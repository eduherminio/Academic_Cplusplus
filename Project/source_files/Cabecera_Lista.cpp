/* Author:      Eduardo Cáceres de la Calle
 * Subject:     Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */

 
#include "../header_files/Cabecera_Lista.hpp"

                                // Definición de variables globales
const float DEF_TEMP= 1111;             // Temperatura por defecto
const int DEF_PID= 99999;               // pid por defecto


/* Nombre:      Dato_Sensor
 * Tipo:        Constructor
 * Función:     Asigna unos valores por defecto a los objetos de la clase Dato_Sensor
 * Otros:
 *              Más tarde nos permitirá distinguir entre los datos cargados desde el archivo de texti (numéricos)
 *              de los datos recibidos de Control (cadenas de caracteres), para darles distinto trato
 */
Dato_Sensor::Dato_Sensor()
{
    m_temp= DEF_TEMP;
    m_pid=  DEF_PID;
    m_time=0;
}


/* Nombre:      Lectura_fichero
 * Tipo:        void
 * Función:     Carga la información de un fichero en el contenedor Historial
 * Argumentos:
 *              @ name          Nombre del ficheor del que leer los datos
 *              @ Historial     Contenedor al que volcar los datos leídos. Paso por referencia
 * Otros:
 *              Permite almacenar los datos en Historial de manera numérica, y no como cadenas de caracteres,
 *              permitiendo una sencilla ampliación de las aplicaciones del programa
 */
void Dato_Sensor::Lectura_fichero(string name, vector<Dato_Sensor>& Historial)
{
    ifstream entrada;
    entrada.open(name,ios::in);

    if(!entrada.good())
    {
        entrada.close();
        throw err_apertura;
    }

    cout<< "\nEl archivo "<<name<<" ha sido abierto correctamente"<<'\n'<<"Cargando datos:\n(...)"<<endl;

    while(!entrada.eof())   // CARGA INICIAL
    {
        Dato_Sensor objeto;

        entrada >> objeto.m_temp;
        entrada >> objeto.m_pid;
        entrada >> objeto.m_time;

        if(entrada.fail())
        {
            cout<<"Entrada fail: "<<entrada<<endl;
            entrada.close();
            throw err_lectura;
        }

        Historial.push_back(objeto);
    }

    entrada.close();

    cout<<"Los datos del fichero "<<name<<" han sido cargados correctamente\n"<<endl;
}


/* Nombre:      Recepcion_datos
 * Tipo:        void
 * Función:     Recibe la cadena de datos de Control, y si procede incorpora esos datos a Historial (como cadenas de caracteres)
 * Argumentos:
 *              @ Historial     Contenedor al que volcar los datos leídos. Paso por referencia
 * Otros:
 *              Tiene en cuenta la posibilidad de que se le soliciten datos a Sensor cuando todavía no ha generado ninguno,
 *              situación debida a la simulación dado que un sensor real siempre estaría tomando datos
 */
void Dato_Sensor::Recepcion_datos(vector<Dato_Sensor> &Historial)
{
    colamsg cola_unica("colaunica", 0, RDWR);

    while(true)
    {
        Dato_Sensor objeto;
        char msg_char[30];

        cout<<"<Lista> Esperando a recibir info"<<endl;
        cola_unica.receive(msg_char, 30*sizeof(char));

        string msg_str=msg_char;

        Tratamiento_info(msg_str, objeto);

        if (objeto.s_temp=="SIGINT")         // Salida del bucle, tras el envío del ctrl+c
            break;
        else if (objeto.s_temp=="100000.00" || objeto.s_temp== "0.00")      // En caso de que se pida info a un sensor demasiado rápido,
            continue;                                                       //no le de tiempo a generar temperatura alguna y mande
                                                                            // el valor por defecto de temp_global o incluso 0.00
        Salida_pantalla(objeto);

        Historial.push_back(objeto);
    }
}


/* Nombre:      Tratamiento_info
 * Tipo:        void
 * Función:     Recibe el paquete de información, y lo trocea para poder incorporarlo a Historial
 * Argumentos:
 *              @ Historial     Contenedor al que volcar los datos leídos. Paso por referencia
 *              @ msg_str       Paquete de información
 * Otros:
 *              El tratamiento que se aplica está hecho pensando en la durabilidad del código:
 *              hace innecesario la adaptación del mismo ante el aumento del las variables que se envían
 */
void Dato_Sensor::Tratamiento_info(string msg_str, Dato_Sensor &objeto)
{
    int sep_1= msg_str.find("|");
    int sep_2= msg_str.find("*");

    string temp_str=msg_str.substr(0, sep_1);
    string pid_str=msg_str.substr(sep_1+1, sep_2 - sep_1-1);
    string time_str=msg_str.substr(sep_2+1);

    objeto.s_temp=temp_str.c_str();
    objeto.s_pid=pid_str.c_str();
    objeto.s_time=time_str.c_str();
}


/* Nombre:      Escritura_fichero
 * Tipo:        void
 * Función:     Transforma la información recibida a un formato numérico, y la escribe (junto con la leída previamente), en un fichero
 * Argumentos:
 *              @ name          Nombre del ficheor del que leer los datos
 *              @ Historial     Contenedor al que volcar los datos leídos. Paso por referencia
 * Otros:
 *              - La transformacióin previa a formato numérico permite la adición de nuevas operaciones al programa
 *                antes de almacenar los datos, como ordenarlos por el valor de la temperatura medida, el pid de sensor que los tomó, etc.
 *              - De hecho, la decisión de usar el formato de sobreescritura en el fichero (en vez de añadir los nuevos datos)
 *                viene motivada por lo anteriormente expuesto.
 */
void Dato_Sensor::Escritura_fichero(string name, vector<Dato_Sensor>& Historial)
{
    cout<<"<Lista> Volcando datos al fichero "<<name<<endl;
    ofstream salida(name, ios::out);
    if(!salida.good())
    {
        salida.close();
        throw err_apertura;
    }

    for(int i=0; i< Historial.size(); i++)  // Transforma sólo los datos nuevos (los que están almacenados en forma de cadenas de caract.
    {                                       // Almacena todos
        // Transformación
        if(Historial[i].m_temp==DEF_TEMP && Historial[i].m_pid==DEF_PID)    // Seleccina los datos nuevos:
        {                                                                   // los datos cargados no tienen los m_temp y m_pid por defecto
            Historial[i].m_temp=atof(Historial[i].s_temp.c_str());
            Historial[i].m_pid=atoi(Historial[i].s_pid.c_str());
            Historial[i].m_time=atoi(Historial[i].s_time.c_str());
        }

        // Introducción
        char temp_salida[5];                                                // Facilita la legibilidad de las temperaturas en el fichero,
        sprintf(temp_salida, "%2.2f", Historial[i].m_temp);                 // incorporando siempre dos decimales

        salida<<temp_salida<<'\t'<<Historial[i].m_pid<<'\t'<<Historial[i].m_time;
        if(i!=Historial.size()-1)   // De manera que en la última línea no introduzca intro
            salida<<'\n';
    }
    cout<<"<Lista> Datos guardados correctamente"<<endl;

    salida.close();
}


/* Nombre:      Salida_pantalla
 * Tipo:        void
 * Función:     Muestra por pantalla la información recibida, tras almacenarla en el archivo
 * Argumentos:
 *              @ objeto        objeto que contiene los datos que se sacarán por pantalla
 * Otros:
 *              Muestra el timestamp en formato legible para el usuario
 */
void Salida_pantalla(Dato_Sensor objeto)
{
    string tiempo;
    Fecha_Hora(atoi((objeto.s_time.c_str()) ), tiempo);

    cout<<"\nRecibido: "<<'\n'<<"pid: "<<objeto.s_pid<<endl;
    cout<<"temp medida: "<<showpos<<objeto.s_temp<<noshowpos<<endl;
    cout<<"tiempo "<<tiempo<<endl;
}


/* Nombre:      Fecha_Hora
 * Tipo:        void
 * Función:     Muestra por pantalla la información recibida, tras almacenarla en el archivo
 * Argumentos:
 *              @ fe_ho         cadena sobre la que se escribirá el timestamp en formato legible. Paso por referencia
 */
void Fecha_Hora(time_t unix_time, string &fe_ho)
{
    struct tm * timeinfo;
    timeinfo = localtime (&unix_time);
    fe_ho= asctime(timeinfo);
}


/* Nombre:      Cierre_IPCs
 * Tipo:        void
 * Función:     Agrupa el cierre de semáforos, memorias compartidas y colas que se crearon en control
 */
void Cierre_IPCs()
{
    semaforo s_envio_lista("slista");                       // Re-apertura previa para que sean reconocidos dentro de esta función
        s_envio_lista.close();
    semaforo s_toma_dato("stomadato");
        s_toma_dato.close();

    colamsg cola_unica("colaunica", 0, RDWR);
        cola_unica.close();

    cout<<"\n<Lista> Finalizado"<<endl;
}


/* Nombre:      ctrl_c                  (función de tratamiento de la señal SIGALRM)
 * Tipo:        void
 * Argumentos:
 *              @ sig    Señal recibida
 * Función:     Capta una señal de terminación, desviando la ejecución de Recepcion_datos para finalizar el proceso
 * Otros:
 *              Es necesario el envío de una señal especila a la cola porque podría llegar a suceder que la ejecución quedara
 *              parada en el 'receive'. En cualquier caso, esto evita la introducción de datos np-deseados en el fichero,
 *              a costa de unos segundos más de espera
 */
void ctrl_c(int sig)
{
    cout<<"<hijo> ctrl+c recibido"<<endl;

    colamsg cola_unica("colaunica", 0, RDWR,30*sizeof(char));

    cola_unica.send("SIGINT|", 30*sizeof(char));    // Permite 'saltarse' el receive de la cola
}
