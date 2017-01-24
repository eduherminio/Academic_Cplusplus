/* Author:      Eduardo Cáceres de la Calle
 * Subject:     Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */

 
#ifndef CABECERA_LISTA
#define CABECERA_LISTA

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <string>

#include "semaforo.hpp"
#include "colamsg.hpp"

using namespace std;
                                    // Declaración de variables globales (y su justificación)
extern const int err_apertura;          // Constantes de error
extern const int err_lectura;
extern const float DEF_TEMP;            // Constantes de definición
extern const int DEF_PID;

class Dato_Sensor
{
public:
        float m_temp;           	// Variables miembro
        int m_pid;
        time_t m_time;

        string s_temp;
        string s_pid;
        string s_time;

    Dato_Sensor();                  // Constructor

    void Lectura_fichero (string name, vector<Dato_Sensor>& Historial);             // Funciones de la clase
    void Recepcion_datos (vector<Dato_Sensor> &Historial);
    void Tratamiento_info(string, Dato_Sensor &objeto);
    void Escritura_fichero(string name, vector<Dato_Sensor>& Historial);
};

void Fecha_Hora(time_t, string &);
void Salida_pantalla(Dato_Sensor);

void Cierre_IPCs();
void ctrl_c(int);                   // Función de tratamiento de señal: SIGIN

#endif // CABECERA_LISTA
