#pragma once
#include <qstring.h>
#include <qvector.h>
#include <qobject.h>
#include <algorithm>
#include <ratio>
#include <cstring>
#include <memory>
#include <future>
#include "Logger.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "plyUtility.h"
#define GLM_ENABLE_EXPERIMENTAL
typedef OpenMesh::PolyMesh_ArrayKernelT<>  Mesh;
class VisObject {
public:
	static void initVisObject();
	static Logger* getLog();
	static Mesh poly;
	VisObject();
	~VisObject();
private:
	static Logger* _log;

	static unsigned int cur_poly;
};