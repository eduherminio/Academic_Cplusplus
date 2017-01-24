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
//Compilar:    g++  miprog.cpp memocomp.cpp semaforo.cpp  -o miprog -lpthread -lrt
//-----------------------------------------------------------------------------
// Este software ha sido desarrollado por Eusebio de la Fuente y
// Rogelio Mazaeda dentro del programa de Innovacion Docente 2014-15
// de la Universidad de Valladolid. Julio 2015.
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>         //exit()
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>         //close()
#include <string>
#include <iostream>

#include "../header_files/memocomp.hpp"

using namespace std;

//Constructor para crear o abrir
memocomp::memocomp(string strname, int crear_o_abrir, int lectura_o_escritura, int msg_len)//abrir 0, crear 1
{
    char name[30];
    char nombreConUID[30];

    strcpy(name,strname.c_str());//pasa de string a char[]
    //para no tener conflicto con otros usarios crea nombre con el uid
    uid_t uid=getuid();
    sprintf(nombreConUID, "/%d",uid);
    strcat(nombreConUID,name);
    strcpy(nombre, nombreConUID);
    if(crear_o_abrir==CREAT)  //si creamos la memoria
    {
        if(lectura_o_escritura==RDONLY) fd = shm_open(nombre, O_CREAT|O_RDONLY, 0644);
        else fd = shm_open(nombre, O_CREAT|O_RDWR, 0644);
    }
    else  //si es abrir la memoria
    {
        if(lectura_o_escritura==RDONLY) fd = shm_open(nombre, O_RDONLY, 0);
        else fd = shm_open(nombre, O_RDWR, 0);
    }
    if (fd==-1) //si falla la creación de memoria
    {
        cout << "Error al crear la memo" << endl;
        exit(-1);
    }
    // Establece el tamaño de la memo compartida
    if (ftruncate(fd, msg_len ) == -1)
    {
        cout <<"Error en ftruncate" << endl;
        exit(-1);
    }
    //Ahora haremos la vinculación de memoria
    if(lectura_o_escritura==RDONLY)
        puntero = mmap(NULL, msg_len, PROT_READ , MAP_SHARED, fd, 0); //se puede leer solo
    else puntero = mmap(NULL, msg_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);//se puede leer y escribir
    if (puntero == MAP_FAILED)
    {
        cout <<"Error en vinculacion de memoria mmap" << endl;
        exit(-1);
    }
}

void * memocomp::getPointer() //Devuelve el puntero a la zona de memo compartida
{
    return(puntero);
}

int memocomp::cerrar()
{
    int error=close(fd);
    return(error);//devuelve -1 si ha habido error al cerrar
}

int memocomp::unlink()
{

    int error=shm_unlink(nombre);
    return(error); //devuelve -1 si ha habido error al destruir cola
}





