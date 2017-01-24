/* Author: 	    Eduardo Cáceres de la Calle
 * Subject: 	Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:	Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */


#ifndef CABECERA_CONTROL
#define CABECERA_CONTROL

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>

#include "semaforo.hpp"
#include "memocomp.hpp"
#include "colamsg.hpp"

using namespace std;

                                        // Declaración de variables globales (y su justificación)
extern vector<int> Sensores_activos;            // Deben ser accesibles desde varios hilos
extern string cadena_global;
extern bool act_alarm;                          // Es activada desde funciones de tratamiento de señales
extern int PATH_MAX;                            // Constante de definición
extern int DEF_MAX_SENSORES;
extern int DEF_FRECUENCIA;


void Creacion_IPCs(int);                // Funciones
void Destruccion_IPCs();
void Extraccion_argumentos(int, char *argv[], int &, int &);
string Extraccion_direccion();
void Permiso_Sensor();
void Envio_info_lista();
void Informacion_Sensor(int, int);
void Recepcion_info_sensor(int);
void Reestructuracion_tras_alarma(int);
void Alarma(int);                              // Función de tratamiento de señal: SIGALRM
void ctrl_c(int);                               // Función de tratamiento de señal: SIGINT

#endif // CABECERA_CONTROL
