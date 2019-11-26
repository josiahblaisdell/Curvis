#include "CurvTriMesh.h"
#include "CurvComputer.h"

CurvTriMesh::CurvTriMesh(): m_Computed(false) { Init(); }

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
}

inline CurvTriMesh::PrincipalCurvatures CurvTriMesh::GetPrincipalCurvatures(const TriMesh::VertexHandle& vh)
{
	PrincipalCurvatures principal;
	float h = this->property(m_Mean, vh).length() * 0.5f;
	float k = this->property(m_Gaussain, vh);
	float delta = sqrt(h * h - k);
	principal.first  = h + delta;
	principal.second = h - delta;
	return principal;
}

bool CurvTriMesh::GenVertexBuffer(float* out_buffer, int* out_size)
{
	if (!m_Computed) { return false; }
	int size = n_vertices() * 3;
	out_buffer = new float[size];
	int i = 0;
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		CurvTriMesh::Point pt = point(v_it);
		out_buffer[i * 3 + 0] = pt[0];
		out_buffer[i * 3 + 1] = pt[1];
		out_buffer[i * 3 + 2] = pt[2];
		i++;
	}
	*out_size = size;
	return true;
}

bool CurvTriMesh::GenVertexNormalBuffer(float* out_buffer, int* out_size)
{
	if (!m_Computed) { return false; }
	int size = n_vertices() * 3;
	out_buffer = new float[size];
	int i = 0;
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f norm = normal(v_it);
		out_buffer[i * 3 + 0] = norm[0];
		out_buffer[i * 3 + 1] = norm[1];
		out_buffer[i * 3 + 2] = norm[2];
		i++;
	}
	*out_size = size;
	return true;
}

bool CurvTriMesh::GenVertexNormalBuffer(std::vector<glm::vec3>* out_buffer, unsigned long long* out_size)
{
	if (!m_Computed) { return false; }
	int size = n_vertices();
	//out_buffer = new float[size];
	int i = 0;
	for (CurvTriMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f norm = normal(v_it);
		//out_buffer[i * 3 + 0] = norm[0];
		//out_buffer[i * 3 + 1] = norm[1];
		//out_buffer[i * 3 + 2] = norm[2];
		//out_buffer->push_back(glm::vec3(norm[0], norm[1], norm[2]));
		out_buffer->push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		i++;
	}
	*out_size = size;
	return true;
}


bool CurvTriMesh::GenVertexMajorDirectionBuffer(float* out_buffer, int* out_size)
{
	return true;
}

bool CurvTriMesh::GenVertexMinorDirectionBuffer(float* out_buffer, int* out_size)
{
	return false;
}
