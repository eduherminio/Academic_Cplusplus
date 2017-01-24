//-----------------------------------------------------------------------------
//        Asignatura INFORMATICA INDUSTRIAL
//        Grado en Ingenieria Electronica Industrial y Automatica
//        Escuela de Ingenierias Industriales. Universidad de Valladolid
//-----------------------------------------------------------------------------
//--------------------------Clase memocomp (memoria compartida) ---------------
//Define la clase memocomp empleando funciones POSIX
//El objetivo es una mayor facilidad para el uso de la memoria compartida:
//  -Para CREAR segmento de memoria compartida:
//     memocomp memo("mimemo", CREAT, RDWR, numBytes);
//                                      //Crea segmento memo compart de numBytes
//                                      //con nombre 'mimemo' para lect y esc
//  -Para ABRIR memoria compartida:
//      memocomp memo("mimemo", OPEN, RDONLY, numBytes);
//                                      //Abre segmento memo compart de numBytes
//                                      //con nombre 'mimemo' para lect solo
//  -Para vincular puntero a memoria compartida:
//      punt= memo.getPointer();        //vincula el puntero al segmento de memo
//  -Para CERRAR memoria compartida:
//      memo.cerrar();                  //Cierra el memoria compartida memo
//  -Para DESTRUIR memoria compartida:
//      memo.unlink();                  //Destruye el memoria compartida memo
//-----------------------------------------------------------------------------
//Compilar:    g++  miprog.cpp memocomp.cpp semaforo.cpp  –o miprog -lpthread -lrt
//-----------------------------------------------------------------------------
// Este software ha sido desarrollado por Eusebio de la Fuente y
// Rogelio Mazaeda dentro del programa de Innovacion Docente 2014-15
// de la Universidad de Valladolid. Julio 2015.
//-----------------------------------------------------------------------------

#ifndef MEMOCOMP
#define MEMOCOMP

#define OPEN 0
#define CREAT 1
#define RDONLY 0            //para lectura solo
#define RDWR 2              //para lectura y escritura

class memocomp
{
    int fd;                 //descriptor fichero memo
    void *puntero;          //puntero a memo compartida
    char nombre[30];        //nombre del fichero
public:
    //Constructor para crear o abrir
    memocomp(std::string strname, int crear_o_abrir, int lectura_o_escritura, int msg_len);
    void *getPointer(); //Devuelve el puntero a la zona de memo compartida
    int cerrar();
    int unlink();
};


#endif

