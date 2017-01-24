/* Author:      Eduardo Cáceres de la Calle
 * Subject:     Informática Industrial
 * Degree:      Industrial Electronics and Automatic Control Engineering
 * University:  Universidad de Valladolid (UVa) - EII
 *
 * Code written in 2016, during my first contact with C++.
 * Uploaded for educational purposes only, don't be too hard on me :)
 * 
 */


#include <iostream>
#include "./header_files/Cabecera_Lista.hpp"

using namespace std;

const int err_apertura  =   1;
const int err_lectura   =   2;

int main()
{
    signal (SIGINT, ctrl_c);            // Tratamiento de la señal

    vector<Dato_Sensor> Historial;      // Nótese: variables no globales gracias al modo de tratamiento de ctrl+c
    string name= "Registro.txt";

    Dato_Sensor llamada;

    try
    {
        llamada.Lectura_fichero(name, Historial);   // Necesario para llamar a funciones de la clase

        llamada.Recepcion_datos(Historial);
    }

    catch(int error)
    {
        if(error==err_apertura)
            cerr<<"<Lista> Error en la apertura del fichero"<<endl;
        else if(error==err_lectura)
            cerr<<"<Lista> Error en la lectura del fichero"<<endl;

        return 0;   // Ya que no hay IPCs que cerrar
    }

    llamada.Escritura_fichero(name, Historial);

    Cierre_IPCs();

    return 0;
}
