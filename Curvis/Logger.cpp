#include "Logger.h"
Logger::Logger(std::string fp)
{
	filepath = new std::string(fp);
}


bool Logger::Write(LOGLEVEL lvl, std::string caller, std::string message)
{
	std::ostringstream os;
	char c_time_string_out[1000];

	time_t t = time(NULL);
	tm * time_str_in = localtime(&t);
	strftime(c_time_string_out, 1000, "%m/%d/%Y %H:%M:%S", time_str_in);

	//dr. dobbs logging http://www.drdobbs.com/cpp/logging-in-c/201804215
	os << std::string(c_time_string_out) + "\t";
	switch (lvl)
	{
	case ERR: os << "ERROR \t";
		break;
	case WARN: os << "WARN \t";
		break;
	case INFO: os << "INFO \t";
		break;
	case DEBUG: os << "DEBUG \t";
		break;
	default: os << "DEBUG \t";
		break;
	}
	os << caller + "\t\t";
	os << message << std::endl;
	return Write(os.str());
}

bool Logger::Write(std::string message) {
	std::ofstream of;
	of.open(filepath->c_str(), std::ios_base::app);

	try {
		of << message;
		of.close();
	}
	catch (std::exception ex) { return false; }
	return true;

}

Logger::~Logger()
{
	delete filepath;
}
