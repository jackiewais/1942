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
int select;

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
	item.Id = "Conectar";
	item.Tipo = "192.168.0.1";
	item.Valor = "65532";
	menu.push_back(item);
	item.Id = "Desconectar";
	item.Tipo = "192.168.0.1";
	item.Valor = "65532";
	menu.push_back(item);
	item.Id = "Salir";
	item.Tipo = "192.168.0.1";
	item.Valor = "65532";
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
	item.Tipo = "192.168.0.1";
	item.Valor = "65532";
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
int connect(menuItem menuSelected)
{
	// TODO: RESTA INTEGRAR CON LA IMPLEMENTACIÓN PROPIAMENTE DICHA.
	cout << "-----" << std::endl;
	cout << "Me intento conectar a: " << menuSelected.Tipo << ":" << menuSelected.Valor << std::endl;
	cout << "-----" << std::endl;
	return 0;
}


// INVOCACIÓN A LA LÓGICA PARA DESCONECTARNOS DEL SERVIDOR.
int disconnect(menuItem menuSelected)
{
	// TODO: RESTA INTEGRAR CON LA IMPLEMENTACIÓN PROPIAMENTE DICHA.
	cout << "-----" << std::endl;
	cout << "Me desconecto de: " << menuSelected.Tipo << ":" << menuSelected.Valor << std::endl;
	cout << "-----" << std::endl;
	return 0;
}


// MÉTODO QUE DEBE CERRAR TODAS LAS OPERACIONES INCONCLUSAS Y CERRAR LA APLICACIÓN PROLIJAMENTE.
int finish()
{
	// TODO: RESTA INTEGRAR CON LA IMPLEMENTACIÓN PROPIAMENTE DICHA.
	cout << "-----" << std::endl;
	cout << "Terminamos la ejecución del programa." << std::endl;
	cout << "-----" << std::endl;
	return -1;
}


// ENVIAMOS UN MENSAJE SEGÚN LA INFORMACIÓN OBTENIDA PREVIAMENTE DEL XML.
int sendMessage(menuItem menuSelected)
{
	// TODO: RESTA INTEGRAR CON LA IMPLEMENTACIÓN PROPIAMENTE DICHA.
	cout << "-----" << std::endl;
	cout << "Enviamos el mensaje: " << menuSelected.Id << endl;
	cout << "Tipo:" << menuSelected.Tipo << std::endl;
	cout << "Valor:" << menuSelected.Valor << std::endl;
	cout << "-----" << std::endl;
	return 0;
}


// CICLAMOS PARA EJECUTAR LOS DIFERENTES MENSAJES QUE PUEDEN ESTAR EN EL XML.
int loop(list<menuItem> items)
{
	cout << "-----" << std::endl;
	cout << "Iniciamos la sentencia Ciclar:" << std::endl;
    list<menuItem>::iterator pos;
    pos = items.begin();
    int identificador = 1;

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
int processResponse(int select)
{
    list<menuItem>::iterator pos;
    pos = menu.begin();
    int identificador = 1;
    menuItem menuSelected;
    while( pos != menu.end())
    {
    	if(identificador == select) menuSelected = *pos;
    	identificador++;
    	pos++;
    }

    int response = 0;

	if(select == 1) response = connect(menuSelected);
	if(select == 2) response = disconnect(menuSelected);
	if(select == 3) response = finish();
	if(select > 3 && select < menu.size()) response = sendMessage(menuSelected);
	if(select >= menu.size()) response = loop(menu);

	return response;
}


// METODO PARA PROBAR LAS EJECUCIONES DE LOS MÉTODOS DEL MENU Y EL LOG.-
void metodoPrueba(){

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
		cin >> select;
		myResponse = processResponse(select);
	}

	cout << "FIN DEL METODO EJEMPLO DE PRUEBA RÁPIDO" << endl << endl;
}

// ==============================================================================

int main()
{
	// para probar, luego borrar.
	metodoPrueba();

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
