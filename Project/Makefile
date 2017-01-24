#
#Author:	Eduardo Cáceres de la Calle
#Subject: 	Informática Industrial
#Degree:	Industrial Electronics and Automatic Control Engineering
#University:	Universidad de Valladolid (UVa) - EII
#
# Code written in 2016, during my first contact with C++.
# Uploaded for educational purposes only, don't be too hard on me :)
# 

#Usage: make [options] [target] ...
#Options:
#  -b, -m                      Ignored for compatibility.
#  -B, --always-make           Unconditionally make all targets.
#...etc

#Reminder:
# ./	current folder
# ../	parent folder

# Searchs for all .hpp in ./header_files
IDIR = ./header_files
vpath %.hpp $(IDIR)

todo: delete clean Sensor Control Lista o

#Ficheros de los que depende
Sensor: Sensor.o Cabecera_Sensor.o semaforo.o memocomp.o colamsg.o
	#Cómo crea esos ficheros
	g++ Sensor.o Cabecera_Sensor.o semaforo.o memocomp.o colamsg.o -o Sensor -lrt -lpthread -pthread

Control: Control.o Cabecera_Control.o semaforo.o memocomp.o colamsg.o
	g++ Control.o Cabecera_Control.o semaforo.o memocomp.o colamsg.o -o Control -lrt -lpthread -pthread

Lista: Lista.o Cabecera_Lista.o semaforo.o memocomp.o colamsg.o
	g++ Lista.o Cabecera_Lista.o semaforo.o memocomp.o colamsg.o -o Lista -lrt -lpthread -pthread

#Compilación de cada uno de esos ficheros
Sensor.o: Sensor.cpp Cabecera_Sensor.hpp  semaforo.hpp memocomp.hpp
	g++ -c -std=c++11 Sensor.cpp

Control.o: Control.cpp Cabecera_Control.hpp semaforo.hpp memocomp.hpp colamsg.hpp
	g++ -c -std=c++11 Control.cpp

Lista.o: Lista.cpp Cabecera_Lista.hpp semaforo.hpp colamsg.hpp
	g++ -c -std=c++11 Lista.cpp

semaforo.o: ./source_files/semaforo.cpp semaforo.hpp
	g++ -c -std=c++11 ./source_files/semaforo.cpp

memocomp.o: ./source_files/memocomp.cpp memocomp.hpp
	g++ -c -std=c++11 ./source_files/memocomp.cpp

colamsg.o: ./source_files/colamsg.cpp colamsg.hpp
	g++ -c -std=c++11 ./source_files/colamsg.cpp

Cabecera_Sensor.o: ./source_files/Cabecera_Sensor.cpp Cabecera_Sensor.hpp
	g++ -c -std=c++11 ./source_files/Cabecera_Sensor.cpp

Cabecera_Control.o: ./source_files/Cabecera_Control.cpp Cabecera_Control.hpp
	g++ -c -std=c++11 ./source_files/Cabecera_Control.cpp

Cabecera_Lista.o: ./source_files/Cabecera_Lista.cpp Cabecera_Lista.hpp
	g++ -c -std=c++11 ./source_files/Cabecera_Lista.cpp


#Elimina los ficheros creados previamente por el usuario ****
clean:
	rm -f /dev/shm/sem.1000*
	rm -f /dev/shm/1000*
	rm -f /dev/mqueue/1000*
cola:
	rm -f /dev/mqueue/1000*
o:
	rm -f *.o
delete:
	rm -f Control
	rm -f Sensor
	rm -f Lista
#1207 para Herón (ssh)
#1000 para Linux en portáfil

