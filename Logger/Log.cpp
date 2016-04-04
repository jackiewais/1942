#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <list>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

#include "Log.h"
#include "../Utils/Util.h"

const char* logFile = "log_file.txt";
const char* logErrorFile = "log_error_file.txt";
Util util;

void Log::writeLine(string line)
{
	cout << line << std::endl;
    std::ofstream log_file(logFile, std::ios_base::out | std::ios_base::app );

    log_file << std::endl;
    log_file << util.currentDateTime() + " || " << line << std::endl;
}

void Log::writeErrorLine(string line)
{

	cout << line;
    std::ofstream log_file(logErrorFile, std::ios_base::out | std::ios_base::app );

    log_file << std::endl;
    log_file << util.currentDateTime() + " || " << line << std::endl;
}


void Log::deleteLine()
{
	// TODO: De momento no tiene un uso determinado.-
}

void Log::writeBlock(list<string> lineList)
{
    std::ofstream log_file(logFile, std::ios_base::out | std::ios_base::app );
    list<string>::iterator pos;
    pos = lineList.begin();
    while( pos != lineList.end())
    {
    	cout << *pos;
    	log_file << std::endl;
    	log_file << util.currentDateTime() + " || " << *pos << std::endl;
    	pos++;
    }
}

void Log::writeErrorBlock(list<string> lineList)
{
    std::ofstream log_file(logErrorFile, std::ios_base::out | std::ios_base::app );
    list<string>::iterator pos;
    pos = lineList.begin();
    while( pos != lineList.end())
    {
    	cout << *pos;
    	log_file << std::endl;
    	log_file << util.currentDateTime() + " || " << *pos << std::endl;
    	pos++;
    }
}

void Log::deleteBlock()
{
	// TODO: De momento no tiene un uso determinado.-
}

// Chequeamos que el archivo exista.
inline bool file_exists (const std::string& name) {
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

void Log::createFile()
{
	// Hacemos un backup del anterior archivo de Log.
	string bName =  "(" + util.currentDateTime() + ")" + logFile;
	const char* backupName = bName.c_str();
	if(file_exists(logFile))
		rename(logFile, backupName);

	// Creamos e inicializamos nunestro nuevo archivos de log.
	std::ofstream outfile (logFile);
	outfile << "Archivo de Log inicializado: " << util.currentDateTime() << std::endl;
	outfile << std::endl;
	outfile.close();

}

void Log::createErrorFile()
{
	// Hacemos un backup del anterior archivo de Log.
	string bName =  "(" + util.currentDateTime() + ")" + logErrorFile;
	const char* backupName = bName.c_str();
	if(file_exists(logErrorFile))
		rename(logFile, backupName);

	// Creamos e inicializamos nunestro nuevo archivos de log.
	std::ofstream outfile (logErrorFile);
	outfile << "Archivo de Log para Errores inicializado: " << util.currentDateTime() << std::endl;
	outfile << std::endl;
	outfile.close();

}

void Log::deleteFile()
{
	// TODO: De momento no tiene un uso determinado.-
}