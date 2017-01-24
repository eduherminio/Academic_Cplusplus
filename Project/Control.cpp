/* Author:      Eduardo C�ceres de la Calle
 * Subject:     Inform�tica Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */


#include <iostream>
#include "./header_files/Cabecera_Control.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    signal (SIGINT, ctrl_c);                        // Rutinas de tratamiento de las se�ales
    signal (SIGALRM, Alarma);

    int MAX_SENSORES, freq, pid_lista;              // Preparaci�n e inicializaci�n

    Extraccion_argumentos(argc, argv, MAX_SENSORES, freq);
    Creacion_IPCs(MAX_SENSORES);

    sleep(1);

    int pid;
    pid=fork();
    if(pid==0)          // Hijo
        execlp("./Lista", Extraccion_direccion().c_str(), NULL);

    else                // Padre
    {                                                               //***Tarea que lleva a cabo cada hilo***| **Relacionado**
        thread hilo_info(Informacion_Sensor, freq, MAX_SENSORES);   // Hilo de recepci�n de informaci�n     |      Sensor
        thread hilo_lista(Envio_info_lista);                        // Hilo de env�o de informaci�n         |      Lista

        Permiso_Sensor();                                           // Hilo de peticiones de alta           |      Sensor
    }
    return 0;
}

