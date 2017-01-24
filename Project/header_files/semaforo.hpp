//-----------------------------------------------------------------------------
//        Asignatura INFORMATICA INDUSTRIAL
//        Grado en Ingenieria Electronica Industrial y Automatica
//        Escuela de Ingenierias Industriales. Universidad de Valladolid
//-----------------------------------------------------------------------------
//--------------------------Clase semaforo-------------------------------------
//Define la clase semaforo empleando funciones POSIX
//El objetivo es una mayor facilidad para el uso de semaforos:
//  -Para CREAR semaforo:
//      semaforo S("misemaforo",valor); //Crea semaf S con nombre 'misemaforo'
//                                      //y lo inicializa a valor
//  -Para ABRIR semaforo:
//      semaforo S("misemaforo");       //Abre semaf S con nombre 'misemaforo'
//                                      //El semaf debe haber sido creado antes
//  -Para SUBIR semaforo:
//      S.up();                         //Sube el semaforo S
//  -Para BAJAR semaforo:
//      S.down();                       //Baja el semaforo S
//  -Para intentar BAJAR semaforo:
//      S.trydown();                       //intenta Bajar el semaforo S
//  -Para CERRAR semaforo:
//      S.close();                      //Cierra el semaforo S
//  -Para DESTRUIR semaforo:
//      S.unlink();                     //Destruye el semaforo S
//-----------------------------------------------------------------------------
//Compilar:    g++  miprog.cpp semaforo.cpp –o miprog -lpthread
//-----------------------------------------------------------------------------
// Este software ha sido desarrollado por Eusebio de la Fuente y
// Rogelio Mazaeda dentro del programa de Innovacion Docente 2014-15
// de la Universidad de Valladolid. Julio 2015.
//-----------------------------------------------------------------------------


#ifndef SEMAFOROS
#define SEMAFOROS

#include <semaphore.h>
#define OPEN 0
#define CREAT 1

class semaforo
{
    sem_t* id;
    char nombre[30];
public:
    semaforo(std::string strname, int val); //Para CREAR semaforo
    semaforo(std::string strname);          //para ABRIR semaforo
    int down();                             //BAJAR
    int trydown();                          //intenta bajar
    int up();                               //SUBIR
    int get();                              //VER valor
    int close();                            //CERRAR
    int unlink();                           //DESTRUIR
};

#endif



