//-----------------------------------------------------------------------------
//        Asignatura INFORMATICA INDUSTRIAL
//        Grado en Ingenieria Electronica Industrial y Automatica
//        Escuela de Ingenierias Industriales. Universidad de Valladolid
//-----------------------------------------------------------------------------
//--------------Clase cola de mensajes colamsg---------------------------------
//Define la clase colamsg empleando funciones POSIX
//El objetivo es una mayor facilidad para el uso de las colas de mensajes:
//  -Para CREAR colamsg:
//      colamsg cola("micola", CREAT, WRONLY, numBytes);
//                      //Crea cola con nombre "micola" para escribir solo.
//                      //numBytes es el tamaño del mensaje
//  -Para ABRIR cola:
//      colamsg cola("micola", OPEN, RDONLY); //Abre para leer solo
//                                      //La cola debe haber sido creada antes
//  -Para MANDAR mensaje a cola:
//      cola.send(punt,numBytes);
//                      //Manda a cola el mensaje de numBytes apuntado por punt
//  -Para RECIBIR mensaje de cola:
//      cola.receive(punt,numBytes);
//                      //Recibe de cola el mensaje de numBytes en punt
//  -Para CERRAR cola:
//      cola.close();                   //Cierra la colamsg cola
//  -Para DESTRUIR cola:
//      cola.unlink();                  //Destruye la colamsg cola
//-----------------------------------------------------------------------------
//Compilar:    g++  miprog.cpp colamsg.cpp  –o miprog -lrt
//-----------------------------------------------------------------------------
// Este software ha sido desarrollado por Eusebio de la Fuente y
// Rogelio Mazaeda dentro del programa de Innovacion Docente 2014-15
// de la Universidad de Valladolid. Julio 2015.
//-----------------------------------------------------------------------------

#ifndef COLASMSG
#define COLASMSG

#define OPEN 0
#define CREAT 1
#define RDONLY 0
#define WRONLY 1
#define RDWR 2
#define NUM_MENSAJES 5

#include <mqueue.h>

class colamsg
{
    mqd_t id;
    char nombre[30];
public:
    colamsg(std::string strname, bool abrir_o_crear , int lectura_o_escritura, size_t msg_len);//abrir 0, crear 1
    colamsg(std::string strname, bool abrir_o_crear , int lectura_o_escritura);//abrir 0, crear 1
    void creat(char name[30], int lectura_o_escritura, size_t msg_len);
    void open(char name[30],int lectura_o_escritura);
    int send(const char *msg_ptr, size_t msg_len  );
    int receive(char *msg_ptr, size_t msg_len  );
    int close();
    int unlink();
};



#endif
