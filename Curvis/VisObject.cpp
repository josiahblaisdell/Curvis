#include "visObject.h"
Logger *VisObject::_log = new Logger("Curvis.log");
PolyMesh VisObject::polymesh;
CurvTriMesh VisObject::curvtrimesh;

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