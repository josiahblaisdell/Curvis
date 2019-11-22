#pragma once
#include <stdio.h>
#include <string>
#include <time.h>
#include <sstream>
#include <fstream>
#include <ostream>
#include <iostream>
enum LOGLEVEL
{
	ERR, WARN, INFO, DEBUG
};

class Logger
{
public:
	bool Write(LOGLEVEL lvl, std::string caller, std::string message);
	Logger(std::string fp = "logfile.log");
	~Logger();
private:
	bool Write(std::string message);
	std::string *filepath;
};