/* Author: 	    Eduardo Cáceres de la Calle
 * Subject: 	Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:	Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */


#include <iostream>
#include "./header_files/Cabecera_Sensor.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    signal (SIGINT, ctrl_c);                                // Rutinas de tratamiento de las señales
    signal (SIGUSR1, Envio_info_control);

    Solicitud_Permiso();

    Generacion_temperatura(argc, argv);

    return 0;   // Que nunca se llega a ejecutar
}
