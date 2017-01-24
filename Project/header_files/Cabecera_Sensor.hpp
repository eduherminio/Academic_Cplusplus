/* Author: 	    Eduardo C�ceres de la Calle
 * Subject: 	Inform�tica Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:	Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */

 
#ifndef CABECERA_SENSOR
#define CABECERA_SENSOR

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <string.h>

#include "semaforo.hpp"
#include "memocomp.hpp"

using namespace std;
                                // Declaraci�n de variables globales (y su justificaci�n)
extern float temp_global;               // Necesario su uso desde Envio_info_control(int)
extern int DEF_FRECUENCIA;              // Constante de definici�n
                                // Funciones
void Solicitud_Permiso();
void Generacion_temperatura(int, char *argv[]);
void Envio_info_control(int);       // Funci�n de tratamiento de se�al: SIGINT
string Generacion_paquete_info();
void ctrl_c(int);                   // Funci�n de tratamiento de se�al: SIGUSR1

#endif // CABECERA_SENSOR
