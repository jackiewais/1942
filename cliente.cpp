
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <limits>
#include <arpa/inet.h>
#include <ctime>
#include <sstream>
#include "Logger/Log.h"
#include "Parser/Parser.h"
#include <string>
using namespace std;

// ==============================================================================

struct  mensaje {
    string Id;
    string Tipo;
    string Valor;
};

unsigned short portNumber;
unsigned short logLevel;
string ipChar;
bool isConnected;

Log log;

list<string> listaMenu;
vector<struct mensaje> listaMensajes;

int socketCliente;

enum messageType {CHAR, INT, DOUBLE, STRING, ERROR};
// ==============================================================================


// GENERAMOS LA ESTRUCTURA DEL MENU A UTILIZAR.
list<string> generateMenu(vector<struct mensaje> mensajes)
{
	//log.writeWarningLine("generateMenu::arrancamos la generación del menu.");
	list<string> menu;
	string item;

	item = "Conectar";
	menu.push_back(item);

	item = "Desconectar";
	menu.push_back(item);

	item = "Salir";
	menu.push_back(item);

	for (unsigned int i = 0; i< mensajes.size(); i++)
	{
		item = mensajes[i].Id;
		menu.push_back(item);
	}

	item = "Ciclar";
	menu.push_back(item);
	//log.writeWarningLine("generateMenu::hemos generado el menu correctamente.");
	return menu;
}


// IMPRIME LA PANTALLA DEL MENU PREVIAMENTE GENERADO.-
void printMenu()
{
	//log.writeWarningLine("printMenu::arrancamos la impresión del menu.");
	cout << std::endl;

	int i = 1;
	for (list<string>::iterator j = listaMenu.begin(); j != listaMenu.end(); j++)
	{
    	cout << i << ") " << *j << std::endl;
    	i++;
    }
	///log.writeWarningLine("printMenu::hemos generado el menu correctamente.");
}


// INVOCACIÓN A LA LÓGICA PARA CONECTARNOS AL SERVIDOR.
int connect()
{
	//log.writeWarningLine("connect::comenzamos el intento de conectar.");
	struct sockaddr_in server;

	if (isConnected)
	{
		log.writeLine("El servidor ya está conectado");
		//log.writeWarningLine("connect::comenzamos el intento de conectar.");
		return 0;
	}

	cout << "-----" << endl;
	cout << "Me intento conectar a: " << ipChar << ":" << portNumber << endl;
	cout << "-----" << endl;

	 //CREO EL SOCKET.
	log.writeLine("Creando Socket... ");
	socketCliente =  socket(AF_INET , SOCK_STREAM , 0);

	if (socketCliente < 0) {
		log.writeErrorLine("ERROR al crear el socket");
		return 1;
	} else {
		log.writeLine("Socket creado");
	}

	//INICIALIZO LAS VARIABLES DEL STRUCK SOCKADDR_IN
	log.writeLine("Preparamos los valores del socket... ");
	server.sin_family = AF_INET;
	server.sin_port = htons(portNumber);
	server.sin_addr.s_addr = inet_addr(ipChar.c_str());

	//HAGO CONNECT CON EL SERVER
	log.writeLine("Conectando... ");
	if (connect(socketCliente,(struct sockaddr *)&server, sizeof(server)) < 0) {
		log.writeErrorLine("ERROR al conectar con el servidor");
		return 1;
	} else {
		char respuestaServer[256];
		memset(respuestaServer,0,256);
		if( recv(socketCliente, respuestaServer , 255 , 0) < 0)
		{
			log.writeErrorLine("ERROR al recibir la respuesta");
			return 1;
		}
		if (strncmp(respuestaServer, "ERROR", 5) == 0){
			log.writeErrorLine("ERROR al conectar con el servidor:" + string(respuestaServer));
			return 1;
		}

		log.writeLine("Conectado correctamente con el servidor");
		//log.writeWarningLine("connect::terminamos el intento de conectar.");

		isConnected = true;
		return 0;
	}
}


// INVOCACIÓN A LA LÓGICA PARA DESCONECTARNOS DEL SERVIDOR.
int disconnect()
{
	//log.writeWarningLine("disconnect::comenzamos el intento de desconectar.");
	if (!isConnected)
	{
		log.writeLine("El servidor ya está desconectado");
		return 0;
	}

	cout << "-----" << endl;
	cout << "Desconectando del servidor" << endl;
	cout << "-----" << endl;

	// ENVÍO MENSAJE DE EXIT Y CIERRO SOCKET
	char message[256];
	memset(message,0,256);
	message[0]= 'q';
	send(socketCliente , message , strlen(message) , 0);
	close(socketCliente);
	log.writeLine("Socket correctamente cerrado.");
	isConnected = false;
	//log.writeWarningLine("disconnect::terminamos el intento de desconectar.");
	return 0;
}


// MÉTODO QUE DEBE CERRAR TODAS LAS OPERACIONES INCONCLUSAS Y CERRAR LA APLICACIÓN PROLIJAMENTE.
int finish()
{
	//log.writeWarningLine("finish::iniciamos la ejecución para finalizar.");
	cout << "-----" << endl;
	cout << "Terminamos la ejecución del programa." << endl;
	cout << "-----" << endl;

	log.writeLine("antes de terminar nos desconectamos del servidor.");
	if (isConnected)
		disconnect();

	//log.writeWarningLine("finish::terminamos la ejecución para finalizar.");
	return -1;
}


// ENVIAMOS UN MENSAJE SEGÚN LA INFORMACIÓN OBTENIDA PREVIAMENTE DEL XML.
int sendMessage(int nro)
{
	//log.writeWarningLine("sendMessage::iniciamos el envio de mensaje.");
	if (!isConnected)
		{
			log.writeLine("El servidor está desconectado. Conéctelo para enviar mensajes");
			return 0;
		}

	int lengthLength = 3;
	int idLength = 10;
	int typeLength = 1;
	int messageSize = lengthLength + idLength + typeLength +
			listaMensajes[nro].Valor.size();

	char message[messageSize + 1];
	char *temp;
	char *msgLength = message;
	char *msgId = message + lengthLength;
	char *msgType = msgId + idLength;
	char *msgData = msgType + typeLength;

	int n;

	/*cout << "-----" << std::endl;
	cout << "Enviamos el mensaje: " << listaMensajes[nro].Id << endl;
	cout << "Tipo:" << listaMensajes[nro].Tipo << endl;
	cout << "Valor:" << listaMensajes[nro].Valor << endl;
	cout << "-----" << endl;*/

	// MANDO UN MENSAJE
	log.writeLine("Enviando mensaje: ID: " + listaMensajes[nro].Id + " - Tipo: " + listaMensajes[nro].Tipo + " - Valor: " + listaMensajes[nro].Valor);
    memset(message,' ',messageSize);

    temp = new char[lengthLength + 1];

    if (messageSize < 100){
    	temp[0] = '0';
    	strcpy(temp + 1, to_string(messageSize).c_str());
    	//temp[1] = to_string(messageSize).c_str();
    }
    else
    	strcpy(temp, to_string(messageSize).c_str());
    	//temp[0] = to_string(messageSize).c_str();

    strcpy(msgLength, temp);

    delete[] temp;

    strcpy(msgId,listaMensajes[nro].Id.c_str());
    memset(msgId + listaMensajes[nro].Id.size(), ' ', idLength - listaMensajes[nro].Id.size());

    if (listaMensajes[nro].Tipo.compare("STRING") == 0)
    	*msgType = 's';
    else if (listaMensajes[nro].Tipo.compare("INT") == 0)
    	*msgType = 'i';
    else if (listaMensajes[nro].Tipo.compare("DOUBLE") == 0)
    	*msgType = 'd';
    else if (listaMensajes[nro].Tipo.compare("CHAR") == 0)
    	*msgType = 'c';
    else
    	return -1;

    strcpy(msgData,listaMensajes[nro].Valor.c_str());

	if( send(socketCliente , message , messageSize , 0) < 0)
	{
		log.writeErrorLine("ERROR al enviar los datos...");
		return 1;
	} else {
		log.writeLine("Datos enviados satisfactoriamente...");
	}

	// RECIBIENDO INFORMACION

	char respuestaServer[256];
	log.writeLine("Recibiendo información...");
	bzero(respuestaServer,256);
	n = recv(socketCliente, respuestaServer , 255 , 0);
	if( n  < 0)
	{
		log.writeErrorLine("ERROR al recibir los datos");
		return 1;
	}else if (n == 0){
		//Server disconnected
		log.writeErrorLine("ERROR: El servidor está desconectado");
		isConnected = false;
	}else{
		log.writeLine("Recibimos la siguiente respuesta del servidor: " + string(respuestaServer));
	}

	//log.writeWarningLine("sendMessage::terminamos el envio de mensaje.");
	return 0;
}


// CICLAMOS PARA EJECUTAR LOS DIFERENTES MENSAJES QUE PUEDEN ESTAR EN EL XML.
int loop()
{
	//log.writeWarningLine("loop::iniciamos el ciclar.");
	unsigned int duracion, i = 0;
	time_t endwait;
	time_t start = time(NULL);
	time_t seconds;

	cout << "Introduzca la duración del ciclo en milisegundos" << endl;
	cin >> duracion;

	cout << "-----" << std::endl;
	cout << "Iniciamos la sentencia Ciclar:" << std::endl;

	seconds = duracion/1000;
	endwait = start + seconds;

	log.writeLine("Loop start time: " + string(ctime(&start)));

	while ((start < endwait) && (isConnected)){
		 //ciclo mensajes
		sendMessage(i);
		i++;
		if (i == listaMensajes.size())
			i = 0;

		start = time(NULL);
	 }
	if (!isConnected){
		log.writeLine("El servidor está desconectado. Conéctelo para enviar mensajes");
		return 0;
	}

	log.writeLine("Loop end time: " + string(ctime(&start)));

	cout << "-----" << endl;
	//log.writeWarningLine("loop:terminamos el ciclar.");
	return 0;
}


// SEGÚN LO QUE ELIJA EL USUARIO, PROCESAMOS UNA OPCIÓN U OTRA.
int processInput(unsigned int input)
{
	//log.writeWarningLine("processInput::iniciamos el procesamiento sobre lo ingresado por el usuario");
    int response;

    if ((input > listaMenu.size()) || (input < 1)){ //input no válido
    	log.writeErrorLine("Error: Introduzca una de las opciones indicadas");
    	response = 1;
    }else if(input == 1)
		response = connect();
	else if (input == 2)
		response = disconnect();
	else if (input == 3)
		response = finish();
	else if(input == listaMenu.size()){
		response = loop();
	}else
		response = sendMessage(input-4);

	//log.writeWarningLine("processInput::terminamos el procesamiento sobre lo ingresado por el usuario");
	return response;
}

inline const char* ToString(Parser::messageType v)
{
    switch (v)
    {
        case CHAR:   return "CHAR";
        case INT:   return "INT";
        case DOUBLE: return "DOUBLE";
        case STRING: return "STRING";
        default:      return "ERROR";
    }
}

void leerXML(){

	Parser::type_datosCliente xml;

	struct mensaje item;
	string path;

	cout << "Por favor, ingrese el nombre del archivo xml a utilizar o 'default' para usar el de defecto" << endl;
	cin >> path;
	if (path == "default"){
		log.writeWarningLine("Se toma el XML de default");
		path = Parser::getDefaultNameClient();
	}

	while (!Parser::fileExists(path.c_str())){
		cout << "Ruta ingresada no válida. Por favor, ingrese el nombre del archivo xml a utilizar o 'default' para usar el de defecto" << endl;
		cin >> path;
		if (path == "default"){
			log.writeWarningLine("Se toma el XML de default");
			path = Parser::getDefaultNameClient();
		}
	}

	xml = Parser::parseXMLCliente(path.c_str(), &log);

	logLevel = xml.logLevel;
	portNumber = xml.puerto;
	ipChar = xml.ip;

	std::map<int,Parser::type_mensaje>::iterator parserIterator;
	for (parserIterator = xml.mensajes->begin(); parserIterator != xml.mensajes->end(); ++parserIterator)
	{
		const char* sTipo = ToString(parserIterator->second.tipo);

	    std::ostringstream ostr;
	    ostr << parserIterator->second.id;
	    std::string sId = ostr.str();

	    std::ostringstream ostr2;
	    ostr2 << parserIterator->second.valor;
	    std::string sValor = ostr2.str();

		item.Id = sId;
		item.Tipo = sTipo;
		item.Valor = sValor;
		listaMensajes.push_back(item);
	}
}

int main(int argc, char *argv[])
{
	int input;
	int myResponse = 0;
	isConnected = false;

	// Inicializar el log.
	log.createFile(3);

	leerXML();

	log.writeLine("generamos el menu dinamico.");
	listaMenu = generateMenu(listaMensajes);

	log.writeLine("imprimimos el menu dinamico.");
	printMenu();

	while(myResponse >= 0)
	{
		//printMenu();
		cout << "Por favor, ingrese una de las siguientes opciones numéricas:" << endl;
		cin >> input;
		if (!cin){ //Validates if is a number
			cout << "Error: Debe ingresar un número" << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			myResponse = 1;
		}else
			myResponse = processInput(input);
	}

	if (isConnected)
		disconnect();

	return 0;
}
