#pragma once

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

struct MyTraits : OpenMesh::DefaultTraits {
	VertexAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color);

	FaceAttributes(OpenMesh::Attributes::Normal);
};
typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits>  PolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits>  TriMesh;