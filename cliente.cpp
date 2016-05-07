
#include <stdio.h>
#include <stdlib.h>
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
#include "Utils/messages.h"
#include "Elemento/Elemento.h"
#include <string>
using namespace std;

#define BUFLEN 1000



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
	/*
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
	*/
}

// ==============================================================================


unsigned short portNumber;
unsigned short logLevel;
unsigned short clientId;
string ipChar;
bool isConnected, isRunning;
Log log;
int socketCliente;

enum messageType {CHAR, INT, DOUBLE, STRING, ERROR};
// ==============================================================================


int receiveMsg(char* buffer){


	char msgLenChar[3];
	int msgLen, rcvLen;

	rcvLen = recv(socketCliente, buffer, BUFLEN -1 , 0);
	if( rcvLen < 0){
		log.writeErrorLine("ERROR al recibir la respuesta. El servidor no responde");
		return 1;
	}


	memcpy(msgLenChar, buffer, 3);
	msgLen = stoi(msgLenChar, nullptr);

	if (rcvLen == msgLen){//full message received.
		return 0;
	}else{//message incomplete.
		int readed = rcvLen;
		while ( readed != msgLen){
			rcvLen = recv(socketCliente, buffer+readed, msgLen-readed, 0);
			readed += rcvLen;
		}

	}

	return 0;
}


// INVOCACIÓN A LA LÓGICA PARA CONECTARNOS AL SERVIDOR.
int connect(map<int,Elemento*> &elementos)
{
	struct sockaddr_in server;

	if (isConnected){
		log.writeLine("El cliente ya fue conectado");
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
		log.writeErrorLine("ERROR al conectar con el servidor ");
		return 1;
	} else {
 		struct timeval timeout;
		 timeout.tv_sec = 180;
		 timeout.tv_usec = 0;

		 if (setsockopt (socketCliente, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
			 log.writeErrorLine("ERROR setting socket rcv timeout");

		 if (setsockopt (socketCliente, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		 	log.writeErrorLine("ERROR setting socket snd timeout");

		char respuestaServer[BUFLEN];
		memset(respuestaServer,0,BUFLEN);

		if (receiveMsg(respuestaServer) != 0)
			return 1;

		int msgQty;
		struct gst** msgArray;

		msgQty = decodeMessages(&msgArray, respuestaServer);
		if (msgQty == -1){
			log.writeErrorLine("ERROR al decodificar la respuesta del servidor"
					+ string(respuestaServer));
			return 1;
		}

		// el primer mensaje recibidio del server tiene que ser de control con el status de la
		// conexion CON_SUCCESS o CON_FAIL

		if (msgArray[0] -> type[0] != msgType::CONTROL){
			log.writeErrorLine("ERROR respuesta del servidor incorrecta"
								+ string(respuestaServer));
			return 1;
		}
		else if (msgArray[0] -> info[0] == (char)command::CON_FAIL){
			log.writeErrorLine("ERROR al conectar con el servidor: conexion rechazada");
			return 1;

		}

		log.writeLine("Conectado correctamente con el servidor");
		clientId = atoi(msgArray[0] -> id);
		msgArray++;
		processMessages(elementos, msgArray, msgQty - 1);
		isConnected = true;

		return 0;
	}
}


// INVOCACIÓN A LA LÓGICA PARA DESCONECTARNOS DEL SERVIDOR.
int disconnect()
{
	if (!isConnected)
	{
		log.writeLine("El servidor ya está desconectado");
		return 0;
	}

	cout << "-----" << endl;
	cout << "Desconectando del servidor" << endl;
	cout << "-----" << endl;

	// ENVÍO MENSAJE DE EXIT Y CIERRO SOCKET

	struct gst* exitMsg;
	char* buffer;
	int bufferLen;

	exitMsg = genAdminGst(clientId, command::DISCONNECT);
	bufferLen = encodeMessages(&buffer, &exitMsg, 1);

	send(socketCliente , buffer , bufferLen , 0);
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

	log.writeLine("antes de terminar nos desconectamos del servidor.");
	if (isConnected)
		disconnect();

	return -1;
}


void interchangeStatus(map<int,Elemento*> &elementos){

	char *bufferSnd, bufferRcv[BUFLEN];
	struct gst* sndMsg, ** rcvMsgs;
	int bufferSndLen, rcvMsgsQty;

	sndMsg = genUpdateGstFromElemento(elementos[clientId]);
	bufferSndLen = encodeMessages(&bufferSnd, &sndMsg, 1);

	send(socketCliente,bufferSnd,bufferSndLen,0);

	memset(bufferRcv,0,BUFLEN);
	if (receiveMsg(bufferRcv) == 0){
		rcvMsgsQty = decodeMessages(&rcvMsgs, bufferRcv);

		if (rcvMsgsQty != -1){
			processMessages(elementos, rcvMsgs, rcvMsgsQty);
		}

	}

}


void processMessages(map<int,Elemento*> &elementos, struct gst** rcvMsgsQty, int msgsQty){

	//TODO

}


int main(int argc, char *argv[])
{
	int input;
	int myResponse = 0;
	isConnected = false;
	isRunning = false;
	map<int,Elemento*> elementos;


	// Inicializar el log.
	log.createFile(3);

	leerXML();

	connect(elementos);

	//elementos[clientId] es el elemento controlado por el cliente
	//dettachGraphicsThread(elementos, elementos[clientId]);
	isRunning = true;
	while(isRunning)
	{
		interchangeStatus(elementos);
	}
	if (isConnected)
		disconnect();

	return 0;
}
