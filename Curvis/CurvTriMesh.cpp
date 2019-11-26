#include "CurvTriMesh.h"
#include "CurvComputer.h"

CurvTriMesh::CurvTriMesh() :
	m_Computed(false)
{
	Init();
}

CurvTriMesh::~CurvTriMesh() { }

bool CurvTriMesh::Boot()
{
	CurvComputer computer;
	bool   flag = computer.Execute(this);
	return flag;
}

void CurvTriMesh::Init()
{
	this->add_property(m_Area);
	this->add_property(m_MixedArea);
	this->add_property(m_Mean);
	this->add_property(m_Gaussain);
	this->add_property(m_CurvatureDir);
	this->add_property(m_Tensor);
}

bool CurvTriMesh::GenVertexBuffer(std::vector<glm::vec4>& out_buffer, unsigned long long& out_size)
{
	if (!m_Computed) { return false; }
	out_size = n_vertices();
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		CurvTriMesh::Point pt = point(v_it);
		out_buffer.push_back(glm::vec4(pt[0], pt[1], pt[2],1.0f));
	}
	return true;
}

bool CurvTriMesh::GenVertexNormalBuffer(std::vector<glm::vec3>& out_buffer, unsigned long long& out_size)
{
	if (!m_Computed) { return false; }
	out_size = n_vertices();
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f norm = normal(v_it);
		out_buffer.push_back(glm::vec3(norm[0], norm[1], norm[2]));
	}
	return true;
}


void CurvTriMesh::GetCurvTriMesh(unsigned long long &verts_n, unsigned long long &norms_n, unsigned long long &colors_n, unsigned long long &indices_n,
							std::vector<glm::vec4> &vertices, std::vector<glm::vec3> &normals, std::vector<glm::vec4> &colors, std::vector<GLuint> &indices,
							std::vector<glm::vec3> &minorcurv, std::vector<glm::vec3> &majorcurv, std::vector<glm::vec3> &meancurv, std::vector<float> &gausscurv) {
	//reset the number of colors, vertices and clear the vertices array
	verts_n   = 0;
	norms_n   = 0;
	colors_n  = 0;
	indices_n = 0;
	vertices.clear();
	normals.clear();
	colors.clear();
	indices.clear();
	//Create the vertex array.
	GenVertexBuffer(vertices, verts_n);
	//create the normals array
	GenVertexNormalBuffer(normals, norms_n);
	//indices for vertices are given by iterating over faces.
	for (CurvTriMesh::FaceIter f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		CurvTriMesh::FaceVertexCCWIter fv_ccw;
		for (fv_ccw = fv_ccwbegin(*f_it); fv_ccw != fv_ccwend(*f_it); ++fv_ccw) {
			GLuint idx = vertex_handle(fv_ccw->idx()).idx();
			indices.push_back(idx);
			indices_n++;
		}
	}
	//create colors for vertices
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it) {
		GLfloat r = 1.f;
		GLfloat g = 0.f;
		GLfloat b = 0.f;
		GLfloat a = 1.f;
		colors.push_back(glm::vec4(r, g, b, a));
		colors_n++;
	}
	//get minor curvature direction
	GenVertexMinorDirectionBuffer(minorcurv);
	//get minor curvature direction
	GenVertexMajorDirectionBuffer(majorcurv);
	//get mean curvature
	GenVertexMeanDirectionBuffer(meancurv);
	//get gauss curvature
	GenVertexGaussDirectionBuffer(gausscurv);
}

bool CurvTriMesh::GenVertexMajorDirectionBuffer(std::vector<glm::vec3>& out_buffer)
{
	if (!m_Computed) { return false; }
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f major = GetCurvatureDirection(v_it).first;
		out_buffer.push_back(glm::vec3(major[0], major[1], major[2]));
	}
	return true;
}


bool CurvTriMesh::GenVertexMinorDirectionBuffer(std::vector<glm::vec3>& out_buffer)
{
	if (!m_Computed) { return false; }
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f minor = GetCurvatureDirection(v_it).second;
		out_buffer.push_back(glm::vec3(minor[0], minor[1], minor[2]));
	}
	return true;
}

bool CurvTriMesh::GenVertexMeanDirectionBuffer(std::vector<glm::vec3>& out_buffer)
{
	if (!m_Computed) { return false; }
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f mean = GetMeanCurvatureNormal(v_it);
		out_buffer.push_back(glm::vec3(mean[0], mean[1], mean[2]));
	}
	return true;
}

bool CurvTriMesh::GenVertexGaussDirectionBuffer(std::vector<float>& out_buffer)
{
	if (!m_Computed) { return false; }
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		float gaussCurv = GetGaussainCurvature(v_it);
		out_buffer.push_back(gaussCurv);
	}
	return true;
}
