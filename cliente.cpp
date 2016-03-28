/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include "Logger/Log.h"
using namespace std;




int main()
{
 int socketCliente;
 struct sockaddr_in server;
 char *mensaje;
 char *respuestaServer[2000];


// Inicializar el log.
Log log;
log.createFile();
log.writeLine("CREANDO SOCKET... ");
 //CREO EL SOCKET.
 cout << "CREANDO SOCKET... ";
socketCliente =  socket(AF_INET , SOCK_STREAM , 0);
if (socketCliente < 0) {
cout << "ERROR"<< endl;
}
cout << "OK" << endl;


//INICIALIZO LAS VARIABLES DEL STRUCK SOCKADDR_IN
server.sin_family = AF_INET;
server.sin_port = htons(8888);
server.sin_addr.s_addr = inet_addr("192.168.0.11");

//HAGO CONNECT CON EL SERVER
cout << "CONECTANDO... ";
if (connect(socketCliente,(struct sockaddr *)&server, sizeof(server)) < 0)
  {
cout << "ERROR" << endl;
return 1;
  }
cout << "OK" << endl;

// MANDO UN MENSAJE
mensaje = "cliente : Hola ";
cout << "ENVIANDO DATOS...";
 if( send(socketCliente , mensaje , strlen(mensaje) , 0) < 0)
   {
       cout << "ERROR" << endl;
       return 1;
   }
cout << "OK" << endl;

// RECIBIENDO INFORMACION
cout << "RECIBIENDO INFORMACION...";
   if( recv(socketCliente, respuestaServer , 2000 , 0) < 0)
   {
       cout << "ERROR" << endl;
       return 1;
   }
 cout << "OK" << endl;
 cout << "RESPUESTA DEL SERVIDOR :"<< endl;
 cout << respuestaServer << endl;



// CIERRO SOCKET
 close(socketCliente);
 cout << "CIERRO SOCKET"<< endl;
return 0;
}
