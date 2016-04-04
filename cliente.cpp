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


// ==============================================================================

struct  menuItem {
    string Id;
    string Tipo;
    string Valor;
};

list<menuItem> menu;
Log log;

// Variables locales relacionadas con el socket.
int socketCliente;
struct sockaddr_in server;
char *mensaje;

// ==============================================================================


// GENERAMOS LA ESTRUCTURA DEL MENU A UTILIZAR.
list<menuItem> generateMenu()
{
	list<menuItem> menu;

	// LLAMO A LA IMPLEMENTACION QUE TRABAJA CON LOS XML.
	// RECIBIREMOS UNA LISTA O UN OBJECTO CON ALGUN FORMATO.
	// Y LUEGO ARMO UNA LISTA CON EL STRUCT GENERADO ARRIBA.
	// EN LOS ELEMENTOS DEL MENU QUE NO SON DINÁMICOS APROVECHO
	// Y RESGUARDO POR EJEMPLO EL IP:PUERTO

	menuItem item;
	item.Id = "Salir";
	item.Tipo = "";
	item.Valor = "";
	menu.push_back(item);

	item.Id = "Mensaje1";
	item.Tipo = "INT";
	item.Valor = "10";
	menu.push_back(item);

	item.Id = "Mensaje2";
	item.Tipo = "STRING";
	item.Valor = "hola mundo";
	menu.push_back(item);

	item.Id = "Ciclar";
	item.Tipo = "";
	item.Valor = "";
	menu.push_back(item);

	return menu;
}


// IMPRIME LA PANTALLA DEL MENU PREVIAMENTE GENERADO.-
void printMenu(list<menuItem> items)
{
    list<menuItem>::iterator pos;
    pos = items.begin();
    int identificador = 1;

    while( pos != items.end())
    {
    	cout << std::endl;
    	cout << identificador << ") " << (*pos).Id << std::endl;

    	identificador++;
    	pos++;
    }
}


// INVOCACIÓN A LA LÓGICA PARA CONECTARNOS AL SERVIDOR.
int connect(char* ip, const char* port)
{
	cout << "-----" << std::endl;
	cout << "Me intento conectar a: " << ip << ":" << port << std::endl;
	cout << "-----" << std::endl;

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
	const char* portChar = port;
	unsigned short portNumber = (unsigned short) strtoul(portChar, NULL, 0);
	server.sin_port = htons(portNumber);
	//EN TIPO RESGUARDE LA IP DEL SERVIDOR.
	const char* ipChar = ip;
	server.sin_addr.s_addr = inet_addr(ipChar);

	//HAGO CONNECT CON EL SERVER
	log.writeLine("CONECTANDO... ");
	if (connect(socketCliente,(struct sockaddr *)&server, sizeof(server)) < 0) {
		log.writeLine("ERROR AL CONECTAR CON EL SERVIDOR.");
		return -1;
	} else {
		log.writeLine("CONECTADOS CORRECTAMENTE CON EL SERVIDOR.");
		return 0;
	}
}


// INVOCACIÓN A LA LÓGICA PARA DESCONECTARNOS DEL SERVIDOR.
int disconnect()
{
	cout << "-----" << std::endl;
	cout << "Desconectando del servidor" << std::endl;
	cout << "-----" << std::endl;

	// CIERRO SOCKET
	close(socketCliente);
	log.writeLine("HEMOS CERRADO CORRECTAMENTE EL SOCKET.");
	return 0;
}


// MÉTODO QUE DEBE CERRAR TODAS LAS OPERACIONES INCONCLUSAS Y CERRAR LA APLICACIÓN PROLIJAMENTE.
int finish()
{
	cout << "-----" << std::endl;
	cout << "Terminamos la ejecución del programa." << std::endl;
	cout << "-----" << std::endl;
	return -1;
}


// ENVIAMOS UN MENSAJE SEGÚN LA INFORMACIÓN OBTENIDA PREVIAMENTE DEL XML.
int sendMessage(menuItem menuSelected)
{

	char respuestaServer[255];

	cout << "-----" << std::endl;
	cout << "Enviamos el mensaje: " << menuSelected.Id << endl;
	cout << "Tipo:" << menuSelected.Tipo << std::endl;
	cout << "Valor:" << menuSelected.Valor << std::endl;
	cout << "-----" << std::endl;

	// MANDO UN MENSAJE
	log.writeLine("ENVIANDO DATOS...");
	mensaje = strdup(menuSelected.Valor.c_str());
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
int loop(list<menuItem> items)
{
	cout << "-----" << std::endl;
	cout << "Iniciamos la sentencia Ciclar:" << std::endl;
    list<menuItem>::iterator pos;
    pos = items.begin();
    unsigned int identificador = 1;

    while( pos != items.end())
    {
    	if(identificador > 3 && identificador < items.size()) sendMessage(*pos);
    	identificador++;
    	pos++;
    }
	cout << "-----" << std::endl;
	return 0;
}


// SEGÚN LO QUE ELIJA EL USUARIO, PROCESAMOS UNA OPCIÓN U OTRA.
int processInput(unsigned int input)
{
    list<menuItem>::iterator pos;
    pos = menu.begin();
    unsigned int identificador = 1;
    menuItem menuSelected;
    while( pos != menu.end())
    {
    	if(identificador == input) menuSelected = *pos;
    	identificador++;
    	pos++;
    }

    int response;

	if(input == 0)
		response = finish();
	else if(input < menu.size())
		response = sendMessage(menuSelected);
	else						//input no válido
		response = loop(menu);

	return response;
}


// METODO PARA PROBAR LAS EJECUCIONES DE LOS MÉTODOS DEL MENU Y EL LOG.-
void metodoPrueba(){

	unsigned int input;

	cout << "METODO EJEMPLO DE PRUEBA RÁPIDO" << endl << endl;

	Log log;
	log.createFile();

	log.writeLine("normal 1");
	log.writeLine("normal 2");
	log.writeLine("normal 3");

	list <string> myList;

	myList.push_back("de lista 1");
	myList.push_back("de lista 2");
	myList.push_back("de lista 3");

	log.writeBlock(myList);

	menu = generateMenu();
	int myResponse = 0;
	while(myResponse >= 0)
	{
		printMenu(menu);
		cout << "Por favor, ingrese una de las siguientes opciones numéricas:" << endl;
		cin >> input;
		myResponse = processInput(input);
	}

	cout << "FIN DEL METODO EJEMPLO DE PRUEBA RÁPIDO" << endl << endl;
}
// ==============================================================================

int main(int argc, char *argv[])
{
	// Inicializar el log.
	log.createFile();
	char *ip;
	char const *port = "5001";

	if (argc!=3 && argc!=2)
		log.writeLine("Cantidad de parámetros inválido");

	ip = argv[1];

	if (argc == 3)
		port = argv[2];

	connect(ip, port);

	// para probar, luego borrar.
	metodoPrueba();

	disconnect();

	return 0;
}
