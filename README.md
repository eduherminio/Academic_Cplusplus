# Academic_Cplusplus
C++ code developed for academic purposes, back in 2015.


## Intro
This is a C++ project which simulates a Temperature Control System compound by a number of temperature sensors and a controller in charge of getting data from them.

Multi-threading, parent-child procceses, ipc's (shm, queues, semaphores) & signal interruptions are used.

ipc's interfaces (included) provided by Eusebio de la Fuente López and Rogelio Mazaeda Echevarría, professors at Universidad de Valladolid (UVa).

**Disclaimer**: code is written in Spanish, an English version will definitely be availabe...sooner or later.


## Project description
### **Sensor**
Firstly, every Sensor process asks Control for permission to pass him data, using a shared memory and semaphores, and waits for that permission to be given.

Once linked, numbers within a specific range are pseudo-randomly generated (with a constant, chosen frequency), pretending to be temperature data.

When a SIGUSR1 signal is received, data is packed in a string and placed in a shared memory so that Control can read them.

When a SIGINT signal is received, it ends its execution.

### **Lista**
1. Firstly, it reads any data available from Registro.txt
2. Secondly, it gets data from Control (indefinitely).
3. When a SIGINT signal is received, it saves all the data to Registro.txt and ends its execution.

### **Control**
Program with parent and child processes. Should be executed in first place.

It's able to deal  with SIGINT signals, closing and destroying ipc's and ending safely the execution of all its threads (making sure Lista has time to save all data) and linked sensors.

It alsos uses SIGALRM to watch time-outs from linked Sensor's.
##### Parent
_Multi-thread_ process:

1. **Permiso_Sensor()**: waits for permission requests from Sensor's, allowing a defined max. number of linked devices. It is able to detect disconnections our lost signal by time-outs. It shares a pid-related memory with Sensor's.
2. **thread hilo_info()**: asks linked Sensor's for info, and gets it. It shares an info-related memory with Sensor's.
3. **thread hilo_lista()**: sends that info to Lista using a queue.

##### child
Executes Lista (same console/window).


## Execution instructions

Nothing should be modified or relocated in /Project. Paths and name of execution files are used in the code, so any change may lead to misbehaviour.

Control should be executed first.

Control arg_MAXSENSORES [default: 2], arg_FREQ [default: 2]
Sensor arg_FREQ [default: 1]
