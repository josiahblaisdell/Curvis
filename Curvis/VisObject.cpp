#include "visObject.h"
Logger *VisObject::_log = new Logger("Curvis.log");
Mesh VisObject::poly;
VisObject::VisObject()
{
}

void VisObject::initVisObject()
{
	int x = 0;
}

Logger* VisObject::getLog()
{
	return _log;
}
VisObject::~VisObject()
{
}