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
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#define GLM_ENABLE_EXPERIMENTAL
struct MyTraits : OpenMesh::DefaultTraits {
	VertexAttributes(OpenMesh::Attributes::Normal |
		OpenMesh::Attributes::Color);

	FaceAttributes(OpenMesh::Attributes::Normal);
};
typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits>  PolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits>  TriMesh;
class VisObject {
public:
	static void initVisObject();
	static Logger* getLog();
	static PolyMesh polymesh;
	static TriMesh trimesh;
	VisObject();
	~VisObject();
private:
	static Logger* _log;

	static unsigned int cur_poly;
};