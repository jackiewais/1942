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
		void* valor;
	};

	struct type_datosCliente {
		char * ip;
		int puerto;
		std::map<int,type_mensaje> * mensajes;
	};

	type_datosServer parseXMLServer(char * nombreArchivo);
	type_datosCliente parseXMLCliente(char * nombreArchivo);

};

