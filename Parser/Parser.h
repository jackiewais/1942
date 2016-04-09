/*
 * Parser.h
 *
 *  Created on: Mar 29, 2016
 *      Author: bigfatpancha
 */


#include <map>

namespace Parser {

	enum messageType {CHAR, INT, DOUBLE, STRING, ERROR};

	struct type_datosServer {
		int cantMaxClientes;
		int puerto;
	};

	struct type_mensaje {
		int id;
		messageType tipo;
		char* valor;
	};

	struct type_datosCliente {
		char * ip;
		int puerto;
		std::map<int,type_mensaje> * mensajes;
	};

	type_datosServer parseXMLServer(const char * nombreArchivo);
	type_datosCliente parseXMLCliente(const char * nombreArchivo);

};


