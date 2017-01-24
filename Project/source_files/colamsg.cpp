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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>

#include "../header_files/colamsg.hpp"

using namespace std;

colamsg::colamsg(string strname, bool abrir_o_crear , int lectura_o_escritura, size_t msg_len)//abrir 0, crear 1
{
    char name[30];
    strcpy(name,strname.c_str());
    if (abrir_o_crear==OPEN)
        open(name, lectura_o_escritura);
    else //CREAT
        creat(name, lectura_o_escritura, msg_len);
}

colamsg::colamsg(string strname, bool abrir_o_crear , int lectura_o_escritura)//abrir 0, crear 1
{
    char name[30];
    strcpy(name,strname.c_str());
    if (abrir_o_crear==OPEN)
        open(name, lectura_o_escritura);
    else //CREAT, error falta msg_len
        printf("ERROR creando cola\n");
}

void colamsg::creat(char name[30], int lectura_o_escritura, size_t msg_len)
{
    struct mq_attr atributos; /* Atributos */
    atributos.mq_maxmsg = NUM_MENSAJES;
    atributos.mq_msgsize = msg_len;
    char nombreConUID[30];
    //para no tener conflicto con otros usarios crea nombre con el uid
    uid_t uid=getuid();
    sprintf(nombreConUID, "/%d",uid);
    strcat(nombreConUID,name);
    strcpy(nombre, nombreConUID);
    if(lectura_o_escritura==RDONLY) id = mq_open(nombre, O_CREAT|O_RDONLY, 0644, &atributos);
    else if(lectura_o_escritura==WRONLY) id = mq_open(nombre, O_CREAT|O_WRONLY, 0644, &atributos);
    else id = mq_open(nombre, O_CREAT|O_RDWR, 0644, &atributos);
}

void colamsg::open(char name[30],int lectura_o_escritura)
{
    char nombreConUID[30];
    //para no tener conflicto con otros usarios crea nombre con el uid
    uid_t uid=getuid();
    sprintf(nombreConUID, "/%d",uid);
    strcat(nombreConUID,name);
    strcpy(nombre, nombreConUID);
    if(lectura_o_escritura==RDONLY) id = mq_open(nombre, O_RDONLY);
    else if(lectura_o_escritura==WRONLY) id = mq_open(nombre, O_WRONLY);
    else id = mq_open(nombre, O_RDWR);
}

int colamsg::send(const char *msg_ptr, size_t msg_len  )
{
    int error=mq_send(id, msg_ptr, msg_len, 0);
    return(error); //devuelve -1 si ha habido error al enviar
}

int colamsg::receive(char *msg_ptr, size_t msg_len  )
{
    int error=mq_receive(id, msg_ptr, msg_len, 0);
    return(error); //devuelve -1 si ha habido error al recibir
}

int colamsg::close()
{
    int error=mq_close(id);
    return(error); //devuelve -1 si ha habido error al cerrar
}

int colamsg::unlink()
{
    int error=mq_unlink(nombre);;
    return(error); //devuelve -1 si ha habido error al destruir cola
}


