/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <ctime>
#include "Logger/Log.h"
using namespace std;


// ==============================================================================

struct  mensaje {
    string Id;
    string Tipo;
    string Valor;
};

unsigned short portNumber;
const char* ipChar;
bool isConnected;

Log log;

list<string> listaMenu;
vector<struct mensaje> listaMensajes;

// Variables locales relacionadas con el socket.
int socketCliente;
struct sockaddr_in server;
char *mensaje;

// ==============================================================================


// GENERAMOS LA ESTRUCTURA DEL MENU A UTILIZAR.
list<string> generateMenu(vector<struct mensaje> mensajes)
{
	list<string> menu;

	// LLAMO A LA IMPLEMENTACION QUE TRABAJA CON LOS XML.
	// RECIBIREMOS UNA LISTA O UN OBJECTO CON ALGUN FORMATO.
	// Y LUEGO ARMO UNA LISTA CON EL STRUCT GENERADO ARRIBA.
	// EN LOS ELEMENTOS DEL MENU QUE NO SON DINÁMICOS APROVECHO
	// Y RESGUARDO POR EJEMPLO EL IP:PUERTO

	string item;

	item = "Conectar";
	menu.push_back(item);

	item = "Desconectar";
	menu.push_back(item);

	item = "Salir";
	menu.push_back(item);

	for (int i = 0; i< mensajes.size(); i++)
	{
		item = mensajes[i].Id;
		menu.push_back(item);
	}

	item = "Ciclar";
	menu.push_back(item);

	return menu;
}


// IMPRIME LA PANTALLA DEL MENU PREVIAMENTE GENERADO.-
void printMenu()
{
	int i = 1;
	for (list<string>::iterator j = listaMenu.begin(); j != listaMenu.end(); j++)
	{
		cout << std::endl;
    	cout << i << ") " << *j << std::endl;
    	i++;
    }
}


// INVOCACIÓN A LA LÓGICA PARA CONECTARNOS AL SERVIDOR.
int connect()
{
	if (isConnected)
	{
		cout << "El servidor ya está conectado" << endl;
		return 0;
	}

	cout << "-----" << endl;
	cout << "Me intento conectar a: " << ipChar << ":" << portNumber << endl;
	cout << "-----" << endl;

	 //CREO EL SOCKET.
	log.writeLine("CREANDO SOCKET... ");
	socketCliente =  socket(AF_INET , SOCK_STREAM , 0);

	if (socketCliente < 0) {
		log.writeLine("ERROR AL CREAR EL SOCKET. ");
		return -1;
	} else {
		log.writeLine("PUDIMOS CREAR EL SOCKET. ");
	}

	//INICIALIZO LAS VARIABLES DEL STRUCK SOCKADDR_IN
	server.sin_family = AF_INET;
	//EN VALOR RESGUARDE EL PUERTO.
	server.sin_port = htons(portNumber);
	//EN TIPO RESGUARDE LA IP DEL SERVIDOR.
	server.sin_addr.s_addr = inet_addr(ipChar);

	//HAGO CONNECT CON EL SERVER
	log.writeLine("CONECTANDO... ");
	if (connect(socketCliente,(struct sockaddr *)&server, sizeof(server)) < 0) {
		log.writeLine("ERROR AL CONECTAR CON EL SERVIDOR.");
		return -1;
	} else {
		log.writeLine("CONECTADOS CORRECTAMENTE CON EL SERVIDOR.");
		isConnected = true;
		return 0;
	}
}


// INVOCACIÓN A LA LÓGICA PARA DESCONECTARNOS DEL SERVIDOR.
int disconnect()
{
	if (!isConnected)
	{
		cout << "El servidor ya está desconectado" << endl;
		return 0;
	}

	cout << "-----" << endl;
	cout << "Desconectando del servidor" << endl;
	cout << "-----" << endl;

	// CIERRO SOCKET
	close(socketCliente);
	log.writeLine("HEMOS CERRADO CORRECTAMENTE EL SOCKET.");
	isConnected = false;
	return 0;
}


// MÉTODO QUE DEBE CERRAR TODAS LAS OPERACIONES INCONCLUSAS Y CERRAR LA APLICACIÓN PROLIJAMENTE.
int finish()
{
	cout << "-----" << endl;
	cout << "Terminamos la ejecución del programa." << endl;
	cout << "-----" << endl;
	return -1;
}


// ENVIAMOS UN MENSAJE SEGÚN LA INFORMACIÓN OBTENIDA PREVIAMENTE DEL XML.
int sendMessage(int nro)
{
	if (!isConnected)
	{
		cout << "El servidor está desconectado. Conéctelo para enviar mensajes"
			 << endl;
		return 0;
	}

	char respuestaServer[255];

	cout << "-----" << std::endl;
	cout << "Enviamos el mensaje: " << listaMensajes[nro].Id << endl;
	cout << "Tipo:" << listaMensajes[nro].Tipo << endl;
	cout << "Valor:" << listaMensajes[nro].Valor << endl;
	cout << "-----" << endl;

	// MANDO UN MENSAJE
	log.writeLine("ENVIANDO DATOS...");
	mensaje = strdup(listaMensajes[nro].Valor.c_str());
	if( send(socketCliente , mensaje , strlen(mensaje) , 0) < 0)
	{
		log.writeLine("ERROR AL ENVIAR DATOS...");
		return -1;
	} else {
		log.writeLine("HEMOS ENVIADO LOS DATOS SATISFACTORIAMENTE...");
	}

	// RECIBIENDO INFORMACION
	log.writeLine("RECIBIENDO INFORMACION...");
	if( recv(socketCliente, respuestaServer , 255 , 0) < 0)
	{
		log.writeLine("ERROR AL RECIBIDR LOS DATOS.");
		return -1;
	}
	log.writeLine("HEMOS RECIBIDO LA SIGUIENTE RESPUESTA DEL SERVIDOR:");
	log.writeLine(respuestaServer);

	return 0;
}


// CICLAMOS PARA EJECUTAR LOS DIFERENTES MENSAJES QUE PUEDEN ESTAR EN EL XML.
int loop(int duracion)
{
	cout << "-----" << std::endl;
	cout << "Iniciamos la sentencia Ciclar:" << std::endl;

	//inicio el reloj
	double time = 0;
	double endTime = (double) duracion / 1000;
	clock_t startTime = clock();

	unsigned int i = 0;
	while (time < endTime)
    {
		cout << time << endl;
    	//ciclo mensajes
		sendMessage(i);
    	i++;
    	if (i == listaMensajes.size())
    		i = 0;

    	//calculo el nuevo tiempo
    	time = ((double)((clock() - startTime))) / ((double) CLOCKS_PER_SEC);
    }
	cout << "-----" << endl;
	return 0;
}


// SEGÚN LO QUE ELIJA EL USUARIO, PROCESAMOS UNA OPCIÓN U OTRA.
int processInput(unsigned int input)
{
    int response;
    unsigned int ms;

    if(input == 1)
		response = connect();
	else if (input == 2)
		response = disconnect();
	else if (input == 3)
		response = finish();
	else if(input == listaMenu.size())
	{
		cout << "Introduzca la duración del ciclo en milisegundos" << endl;
		cin >> ms;
		response = loop(ms);
	}
	else if ((input > listaMenu.size()) || (input < 1))						//input no válido
		response = -2;
	else
		response = sendMessage(input-3);
	return response;
}

void leerXMLMock(){

	portNumber = 5001;
	ipChar = "127.0.0.1";

	struct mensaje item;

	item.Id = "Mensaje1";
	item.Tipo = "INT";
	item.Valor = "10";
	listaMensajes.push_back(item);

	item.Id = "Mensaje2";
	item.Tipo = "STRING";
	item.Valor = "hola mundo";
	listaMensajes.push_back(item);

}

int main(int argc, char *argv[])
{
	unsigned int input;
	int error;

	cout << "METODO EJEMPLO DE PRUEBA RÁPIDO" << endl << endl;

	// Inicializar el log.

	log.createFile();

	//error = leerXML()
	leerXMLMock();
	listaMenu = generateMenu(listaMensajes);

	isConnected = false;
	int myResponse = 0;
	while(myResponse >= 0)
	{
		printMenu();
		cout << "Por favor, ingrese una de las siguientes opciones numéricas:" << endl;
		cin >> input;
		myResponse = processInput(input);
	}

	cout << "FIN DEL METODO EJEMPLO DE PRUEBA RÁPIDO" << endl << endl;

	if (isConnected)
		disconnect();

	return 0;
}
