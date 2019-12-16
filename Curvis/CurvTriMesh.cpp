#include "CurvTriMesh.h"
#include "CurvComputer.h"

CurvTriMesh::CurvTriMesh() :
	m_Computed(false)
{   Init();  }

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

	//josiah 12/15/2019
	this->add_property(m_normal_angle_weighted);
	this->add_property(m_normal_sphere_inscribed);
	this->add_property(m_normal_area_weighted);
	this->add_property(m_normal_gaussian_curvature);
	this->add_property(m_normal_mean_curvature);
	this->add_property(m_gaussian_curvature);
	this->add_property(m_total_mesh_angle_defect);
	this->add_property(m_scalar_mean_curvature);
	this->add_property(m_circumcentric_dual_area);
	this->add_property(m_principal_curvatures);
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
		//out_buffer.push_back(glm::vec3(1, 0, 0));
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

//h is the corner we are computing the angle for
//returns angle between 0 and pi.
float CurvTriMesh::angle(CurvTriMesh::VertexHandle h, CurvTriMesh::HalfedgeHandle heh)
{

	CurvTriMesh::Point p = point(h);
	//previous halfedge
	CurvTriMesh::VertexHandle uh_from = this->from_vertex_handle(heh);
	CurvTriMesh::VertexHandle uh_to = this->to_vertex_handle(heh);
	//next halfedge
	CurvTriMesh::VertexHandle vh_from = this->from_vertex_handle(this->next_halfedge_handle(heh));
	CurvTriMesh::VertexHandle vh_to = this->to_vertex_handle(this->next_halfedge_handle(heh));
	OpenMesh::Vec3f u = (point(uh_to) - point(uh_from)).normalize();
	OpenMesh::Vec3f v = (point(vh_from) - point(vh_to)).normalize();
	return acosf(glm::max(-1.0f, glm::min(1.0f, OpenMesh::dot(u, v))));
}

//this returns the angle between the two faces that share the halfedge heh.
float CurvTriMesh::dihedralAngle(CurvTriMesh::HalfedgeHandle heh)
{
	//Face to the left of the half edge when facing the direction of the half edge.
	CurvTriMesh::FaceHandle fh = CurvTriMesh::PolyConnectivity::face_handle(heh);
	//Face to the right of the half edge when facing the direction of the half edge
	CurvTriMesh::FaceHandle opp_fh = CurvTriMesh::PolyConnectivity::opposite_face_handle(heh);
	OpenMesh::Vec3f n_ijk = normal(fh).normalized();
	OpenMesh::Vec3f n_jil = normal(opp_fh).normalized();
	OpenMesh::Vec3f e_ij = (point(to_vertex_handle(heh)) - point(from_vertex_handle(heh))).normalize();
	float nijk_dot_njil = OpenMesh::dot(n_ijk, n_jil);
	float sintheta = OpenMesh::dot(OpenMesh::cross(n_ijk, n_jil), e_ij);
}

//computes the normal at a vertex using the normals of the surrounding points in the one-ring.
OpenMesh::Vec3f CurvTriMesh::vertexNormalAngleWeighted(CurvTriMesh::VertexHandle vh)
{
	OpenMesh::Vec3f n(0.0f,0.0f,0.0f);
	//for each outgoing halfedge at the point vh
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		CurvTriMesh::HalfedgeHandle heh = halfedge_handle(v_ohe_iter);
		//get the corner point at the other end of the halfedge
		OpenMesh::Vec3f corner_pt(point(to_vertex_handle(heh)));
		FaceHandle fh = face_handle(heh);
		OpenMesh::Vec3f f_normal = normal(fh);
		//heh is prev
		float theta = angle(to_vertex_handle(heh),heh);
		n = n + theta * f_normal;
	}
	return n.normalize();
}
//  
//v2___v1____v0
//  |e01|  /|
//	| \ | / |
//  |   v0  |
//  | / | \ |
//  |/_e04_\|
// v3  v4   v5
OpenMesh::Vec3f CurvTriMesh::vertexNormalSphereInscribed(CurvTriMesh::VertexHandle vh)
{
	OpenMesh::Vec3f n(0.0f,0.0f,0.0f);
	OpenMesh::Vec3f p = point(vh);
	//for each outgoing halfedge at the point vh
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		CurvTriMesh::HalfedgeHandle next_heh = next_halfedge_handle(v_ohe_iter);
		CurvTriMesh::HalfedgeHandle prev_heh = prev_halfedge_handle(v_ohe_iter);
		//get the point at the other end of the halfedge
		OpenMesh::Vec3f next_he_pt(point(to_vertex_handle(next_heh)));
		OpenMesh::Vec3f prev_he_pt(point(to_vertex_handle(prev_heh)));
		OpenMesh::Vec3f eij = prev_he_pt - p;
		OpenMesh::Vec3f eik = next_he_pt - p;
		n = n + (1 / (eij.sqrnorm() * eik.sqrnorm())) * OpenMesh::cross(eij, eik);
	}
	return n.normalize();
}

//returns the normal vector weighted by the areas of the surrounding triangles.
OpenMesh::Vec3f CurvTriMesh::vertexNormalAreaWeighted(CurvTriMesh::VertexHandle vh)
{
	OpenMesh::Vec3f n(0.0f,0.0f,0.0f);
	for (CurvTriMesh::VertexFaceIter vf_iter = vf_begin(vh); vf_iter != vf_end(vh); vf_iter++) {
		OpenMesh::Vec3f face_normal = normal(vf_iter);
		float face_area = GetArea(vf_iter.handle());
		n += face_area * face_normal;
	}
	return n.normalize();
}

//returns a normal at a vertex weighted by the gaussian curvature of adjacent outgoing halfedges.
//This is K*N
OpenMesh::Vec3f CurvTriMesh::vertexNormalGaussianCurvature(CurvTriMesh::VertexHandle vh)
{
	OpenMesh::Vec3f n(0.0f, 0.0f, 0.0f);
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		OpenMesh::Vec3f from_vertex = point(from_vertex_handle(v_ohe_iter));
		OpenMesh::Vec3f to_vertex = point(to_vertex_handle(v_ohe_iter));
		float edge_length = (to_vertex - from_vertex).length();
		float w = .5f * dihedralAngle(v_ohe_iter.handle()) / edge_length;
		n = n + w * (to_vertex - from_vertex);
	}
	return n.normalize();
}
//Returns the mean normal curvature at a point in the mesh
//this is H*N
OpenMesh::Vec3f CurvTriMesh::vertexNormalMeanCurvature(CurvTriMesh::VertexHandle vh)
{
	OpenMesh::Vec3f n(0.0f, 0.0f, 0.0f);
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		float w = 0.5f * ((float)cotan(v_ohe_iter.handle()) + (float)cotan(opposite_halfedge_handle(v_ohe_iter.handle())));
		n = n + w*(point(to_vertex_handle(v_ohe_iter)) - point(from_vertex_handle(v_ohe_iter)));
	}
	return n.normalize();
}

//returns scalar mean curvature H
float CurvTriMesh::scalarMeanCurvature(CurvTriMesh::VertexHandle vh)
{
	float mean_curv = 0.0f;
	//for each outgoing halfedge at the point vh
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		CurvTriMesh::HalfedgeHandle heh = halfedge_handle(v_ohe_iter);
		OpenMesh::Vec3f e_ij = (point(to_vertex_handle(heh)) - point(from_vertex_handle(heh)));
		float theta = dihedralAngle(v_ohe_iter.handle());
		mean_curv  += .5f*theta*e_ij.length();
	}
	return mean_curv;
}

//returns the scalar gaussian curvature K
float CurvTriMesh::angleDefect(CurvTriMesh::VertexHandle vh)
{
	float defect = 0.0f;
	//for each outgoing halfedge at the point vh
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		CurvTriMesh::HalfedgeHandle heh = halfedge_handle(v_ohe_iter);
		float theta = angle(to_vertex_handle(heh),heh);
		defect += theta;
	}
	return 2 * 3.14159 - defect;
}

float CurvTriMesh::totalAngleDefect()
{
	float tot_defect = 0.0f;
	for (CurvTriMesh::VertexIter v_iter = vertices_begin(); v_iter != vertices_end(); v_iter++) {
		tot_defect += angleDefect(v_iter.handle());
	}
	return tot_defect;
}

float CurvTriMesh::circumcentricDualArea(CurvTriMesh::VertexHandle vh)
{
	float circ_area = 0.0f;
	for (CurvTriMesh::VertexOHalfedgeIter v_ohe_iter = voh_begin(vh); v_ohe_iter != voh_end(vh); v_ohe_iter++) {
		CurvTriMesh::HalfedgeHandle prev_heh = next_halfedge_handle(opposite_halfedge_handle(v_ohe_iter));
		float prev_heh_length = (point(to_vertex_handle(prev_heh)) - point(from_vertex_handle(prev_heh))).length();
		CurvTriMesh::HalfedgeHandle heh = halfedge_handle(v_ohe_iter);
		float heh_length = (point(to_vertex_handle(heh)) - point(from_vertex_handle(heh))).length();
		float cot_alpha = cotan(prev_heh);
		float cot_beta = cotan(heh);
		circ_area += (1.0f / 8.0f) * (prev_heh_length * cot_alpha + heh_length * cot_beta);
	}
	return circ_area;

}



float CurvTriMesh::cotan(CurvTriMesh::HalfedgeHandle heh)
{
	if (CurvTriMesh::is_boundary(heh)) {
		return 0.0f;
	}
	CurvTriMesh::HalfedgeHandle next_heh = next_halfedge_handle(heh);
	CurvTriMesh::HalfedgeHandle prev_heh = prev_halfedge_handle(heh);
	OpenMesh::Vec3f u = point(to_vertex_handle(prev_heh)) - point(from_vertex_handle(prev_heh));
	OpenMesh::Vec3f v = point(from_vertex_handle(next_heh)) - point(to_vertex_handle(next_heh));
	return OpenMesh::dot(u, v) / (OpenMesh::cross(u, v).norm());
	
}

CurvTriMesh::PrincipalCurvatures CurvTriMesh::principleCurvatures(VertexHandle vh)
{
	float H = scalarMeanCurvature(vh);
	float K = angleDefect(vh);
	float minor = H - sqrt(H * H - K);
	float major = H + sqrt(H * H - K);
	return CurvTriMesh::PrincipalCurvatures(std::make_pair(minor, major));
}



//

