
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
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

int socketCliente;

Parser::type_datosCliente xml;

enum messageType {CHAR, INT, DOUBLE, STRING, ERROR};
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
	cout << std::endl;

	int i = 1;
	for (list<string>::iterator j = listaMenu.begin(); j != listaMenu.end(); j++)
	{
    	cout << i << ") " << *j << std::endl;
    	i++;
    }
}


// INVOCACIÓN A LA LÓGICA PARA CONECTARNOS AL SERVIDOR.
int connect()
{
	struct sockaddr_in server;

	if (isConnected)
	{
		cout << "El servidor ya está conectado" << endl;
		return 0;
	}

	cout << "-----" << endl;
	cout << "Me intento conectar a: " << ipChar << ":" << portNumber << endl;
	cout << "-----" << endl;

	 //CREO EL SOCKET.
	log.writeLine("Creando Socket... ");
	socketCliente =  socket(AF_INET , SOCK_STREAM , 0);

	if (socketCliente < 0) {
		log.writeLine("ERROR al crear el socket");
		return 1;
	} else {
		log.writeLine("Socket creado");
	}

	//INICIALIZO LAS VARIABLES DEL STRUCK SOCKADDR_IN
	server.sin_family = AF_INET;
	//EN VALOR RESGUARDE EL PUERTO.
	server.sin_port = htons(portNumber);
	//EN TIPO RESGUARDE LA IP DEL SERVIDOR.
	server.sin_addr.s_addr = inet_addr(ipChar);

	//HAGO CONNECT CON EL SERVER
	log.writeLine("Conectando... ");
	if (connect(socketCliente,(struct sockaddr *)&server, sizeof(server)) < 0) {
		log.writeLine("ERROR al conectar con el servidor");
		return 1;
	} else {
		char respuestaServer[256];
		memset(respuestaServer,0,256);
		if( recv(socketCliente, respuestaServer , 255 , 0) < 0)
		{
			log.writeLine("ERROR al recibir la respuesta");
			return 1;
		}
		if (strncmp(respuestaServer, "ERROR", 5) == 0){
			log.writeLine("ERROR al conectar con el servidor:" + string(respuestaServer));
			return 1;
		}

		log.writeLine("Conectado correctamente con el servidor");
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

	// ENVÍO MENSAJE DE EXIT Y CIERRO SOCKET
	char message[256];
	memset(message,0,256);
	message[0]= 'q';
	send(socketCliente , message , strlen(message) , 0);
	close(socketCliente);
	log.writeLine("Socket correctamente cerrado.");
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

	cout << "-----" << std::endl;
	cout << "Enviamos el mensaje: " << listaMensajes[nro].Id << endl;
	cout << "Tipo:" << listaMensajes[nro].Tipo << endl;
	cout << "Valor:" << listaMensajes[nro].Valor << endl;
	cout << "-----" << endl;

	// MANDO UN MENSAJE
	log.writeLine("Enviando datos...");
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
    temp = new char();

    strcpy(msgId,listaMensajes[nro].Id.c_str());

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
		log.writeLine("ERROR al enviar los datos...");
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
		log.writeLine("ERROR al recibir los datos");
		return 1;
	}else if (n == 0){
		//Server disconnected
		log.writeLine("ERROR: El servidor está desconectado");
		isConnected = false;
	}else{
		log.writeLine("Recibimos la siguiente respuesta del servidor: " + string(respuestaServer));
	}

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

    if ((input > listaMenu.size()) || (input < 1)){ //input no válido
    	    	cout << "Error: Introduzca una de las opciones indicadas" << endl;
    	    	response = 1;
    }else if(input == 1)
		response = connect();
	else if (input == 2)
		response = disconnect();
	else if (input == 3)
		response = finish();
	else if(input == listaMenu.size()){
		cout << "Introduzca la duración del ciclo en milisegundos" << endl;
		cin >> ms;
		response = loop(ms);
	}else
		response = sendMessage(input-4);

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

void leerXMLMock(){


	struct mensaje item;
	string path;
	cout << "Por favor, ingrese el nombre del archivo xml a utilizar:" << endl;
	cin >> path;
	char* myPath = const_cast<char*>(path.c_str());
	xml = Parser::parseXMLCliente(myPath);

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

	// Inicializar el log.
	log.createFile();

	//error = leerXML()
	leerXMLMock();
	listaMenu = generateMenu(listaMensajes);

	isConnected = false;
	int myResponse = 0;
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
