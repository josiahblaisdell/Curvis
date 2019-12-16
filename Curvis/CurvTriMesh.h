#pragma once
#include "typdef.h"
#include <glm\glm.hpp>
#include "glew.h"
#include <vector>

class CurvTriMesh: public TriMesh
{
public:
	typedef std::pair<float, float> PrincipalCurvatures;
	typedef std::pair<OpenMesh::Vec3f, OpenMesh::Vec3f> CurvatureDirection;
	struct Tensor //Symetric
	{
		float a, b, c;
		Tensor() : a(0.0f), b(0.0f), c(0.0f) { }
		Tensor(float _a, float _b, float _c) : a(_a), b(_b), c(_c) { }
	};
private:
	// this face property stores the computed area
	OpenMesh::FPropHandleT<float> m_Area;
	// this vertex property stores the computed area
	OpenMesh::VPropHandleT<float> m_MixedArea;
	// this vertex property stores the computed gussain curvature
	OpenMesh::VPropHandleT<float> m_Gaussain;
	// this vertex property stores the computed mean curvature normal
	OpenMesh::VPropHandleT<OpenMesh::Vec3f>    m_Mean;
	// this vertex property stores the computed direction of gussain curvature
	OpenMesh::VPropHandleT<CurvatureDirection> m_CurvatureDir;
	// this vertex property stores the computed direction of gussain curvature
	OpenMesh::VPropHandleT<Tensor> m_Tensor;
	

	//josiah 12/15/2019: added another way to compute the above but don't want to delete valid code.
	OpenMesh::VPropHandleT<OpenMesh::Vec3f> m_normal_angle_weighted;
	OpenMesh::VPropHandleT<OpenMesh::Vec3f> m_normal_sphere_inscribed;
	OpenMesh::VPropHandleT<OpenMesh::Vec3f> m_normal_area_weighted;
	OpenMesh::VPropHandleT<OpenMesh::Vec3f> m_normal_gaussian_curvature;
	OpenMesh::VPropHandleT<OpenMesh::Vec3f> m_normal_mean_curvature;
	OpenMesh::VPropHandleT<float> m_gaussian_curvature;
	OpenMesh::VPropHandleT<float> m_total_mesh_angle_defect;
	OpenMesh::VPropHandleT<float> m_scalar_mean_curvature;
	OpenMesh::VPropHandleT<float> m_circumcentric_dual_area;
	OpenMesh::VPropHandleT<CurvTriMesh::PrincipalCurvatures> m_principal_curvatures;

public:
	CurvTriMesh();
	~CurvTriMesh();
	/*Compute Curvature*/
	bool Boot();
private:
	/*Load */
	void Init();
private:
	friend class CurvComputer;
	bool  m_Computed;
	inline void  SetArea(const TriMesh::FaceHandle& fh, float value) { this->property(m_Area, fh) = value; }
	inline void  SetMixedArea(const TriMesh::VertexHandle& vh, float value) { this->property(m_MixedArea, vh) = value; }
	inline void  SetGaussain(const TriMesh::VertexHandle& vh, float value) { this->property(m_Gaussain, vh) = value; }
	inline void  SetMean(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_Mean, vh) = value; }
	inline void  SetCurvatureDirection(const TriMesh::VertexHandle& vh, CurvatureDirection& value) { this->property(m_CurvatureDir, vh) = value; }
	inline void  SetTensor(const TriMesh::VertexHandle& vh, Tensor& value) { this->property(m_Tensor, vh) = value; }

	//josiah added 12-15-2019
	inline void  set_normal_angle_weighted(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f &value) { this->property(m_normal_angle_weighted, vh) = value; }
	inline void  set_normal_sphere_inscribed(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_normal_sphere_inscribed, vh) = value; }
	inline void  set_normal_area_weighted(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_normal_area_weighted, vh) = value; }
	inline void  set_normal_gaussian_curvature(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_normal_gaussian_curvature, vh) = value; }
	inline void  set_normal_mean_curvature(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_normal_mean_curvature, vh) = value; }
	inline void  set_gaussian_curvature(const TriMesh::VertexHandle& vh, float value) { this->property(m_MixedArea, vh) = value; }
	inline void  set_total_mesh_angle_defect(const TriMesh::VertexHandle& vh, float value) { this->property(m_total_mesh_angle_defect, vh) = value; }
	inline void  set_scalar_mean_curvature(const TriMesh::VertexHandle& vh, float value) { this->property(m_scalar_mean_curvature, vh) = value; }
	inline void  set_circumcentric_dual_area(const TriMesh::VertexHandle& vh, float value) { this->property(m_circumcentric_dual_area, vh) = value; }
public:

	inline float GetArea(const TriMesh::FaceHandle& fh) { return this->property(m_Area, fh); }

	inline float GetMixedArea(const TriMesh::VertexHandle& vh) { return this->property(m_MixedArea, vh); }

	inline float GetGaussainCurvature(const TriMesh::VertexHandle& vh) { return this->property(m_Gaussain, vh); }

	inline float GetMeanCurvature(const TriMesh::VertexHandle& vh) { return this->property(m_Mean, vh).norm() * 0.5f; }

	inline OpenMesh::Vec3f GetMeanCurvatureNormal(const TriMesh::VertexHandle& vh) { return this->property(m_Mean, vh); }

	inline CurvatureDirection GetCurvatureDirection(const TriMesh::VertexHandle& vh) { return this->property(m_CurvatureDir, vh); }

	inline PrincipalCurvatures  GetPrincipalCurvatures(const TriMesh::VertexHandle& vh)
	{
		
		PrincipalCurvatures principal;
		float h = this->property(m_Mean, vh).norm() * 0.5f;
		float k = this->property(m_Gaussain, vh);
		float delta = sqrt(h * h - k);
		principal.first = h + delta;
		principal.second = h - delta;
		return principal;
	}

	inline Tensor GetTensor(const TriMesh::VertexHandle& vh) { return this->property(m_Tensor, vh); }

	//josiah added 12-15-2019:
	inline OpenMesh::Vec3f get_normal_angle_weighted(const TriMesh::VertexHandle& vh) { return this->property(m_normal_angle_weighted, vh); }
	inline OpenMesh::Vec3f get_normal_sphere_inscribed(const TriMesh::VertexHandle& vh) { return this->property(m_normal_sphere_inscribed, vh); }
	inline OpenMesh::Vec3f  get_normal_area_weighted(const TriMesh::VertexHandle& vh) { return this->property(m_normal_area_weighted, vh); }
	inline OpenMesh::Vec3f  get_normal_gaussian_curvature(const TriMesh::VertexHandle& vh) { return this->property(m_normal_gaussian_curvature, vh); }
	inline OpenMesh::Vec3f  get_normal_mean_curvature(const TriMesh::VertexHandle& vh) { return this->property(m_normal_mean_curvature, vh); }
	inline float  get_gaussian_curvature(const TriMesh::VertexHandle& vh) { return this->property(m_MixedArea, vh); }
	inline float  get_total_mesh_angle_defect(const TriMesh::VertexHandle& vh) { return this->property(m_total_mesh_angle_defect, vh); }
	inline float  get_scalar_mean_curvature(const TriMesh::VertexHandle& vh) { return this->property(m_scalar_mean_curvature, vh); }
	inline float  get_circumcentric_dual_area(const TriMesh::VertexHandle& vh) { return this->property(m_circumcentric_dual_area, vh); }
	inline PrincipalCurvatures get_principal_curvatures(const TriMesh::VertexHandle& vh) {
		float H = get_scalar_mean_curvature(vh);
		float K = get_gaussian_curvature(vh);
		float minor = H - sqrt(H * H - K);
		float major = H + sqrt(H * H - K);
		return CurvTriMesh::PrincipalCurvatures(std::make_pair(minor, major));
	}
public:

	bool GenVertexBuffer(std::vector<glm::vec4>& out_buffer, unsigned long long& out_size);

	bool GenVertexNormalBuffer(std::vector<glm::vec3>& out_buffer, unsigned long long& out_size);

	bool GenVertexMajorDirectionBuffer(std::vector<glm::vec3>& out_buffer);

	bool GenVertexMinorDirectionBuffer(std::vector<glm::vec3>& out_buffer);

	bool GenVertexMeanDirectionBuffer(std::vector<glm::vec3>& out_buffer);

	bool GenVertexGaussDirectionBuffer(std::vector<float>& out_buffer);

	//float dihedralAngle();
	//verts_n: number of vertices in mesh
	//norms_n: number of normals in mesh
	//colors_n: number of normals in mesh
	//indices_n: number of indices in the mesh
	//vertices: the vector that stores the vertices in the mesh
	//normals: the vector that stores the nomrals in the mesh.
	//colors: the vector that stores the colors in the mesh
	//indices: the vector that stores the indices into the vertex array for the mesh
	//minorcurv: vector of minor curvature for each vertex
	//majorcurv: vector of major curvature for each vertex
	//meancurv: vector of mean curvature for each vertex
	//Create the colors for the curvtrimesh
	void GetCurvTriMesh(unsigned long long & verts_n, unsigned long long & norms_n, unsigned long long & colors_n, unsigned long long & indices_n, 
						std::vector<glm::vec4>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec4>& colors, std::vector<GLuint>& indices, 
						std::vector<glm::vec3>& minorcurv, std::vector<glm::vec3>& majorcurv, std::vector<glm::vec3>& meancurv, std::vector<float>& gausscurv);

	//josiah 12-14-2019: added these following http://brickisland.net/DDGSpring2019/category/assignments/?order=desc
	float angle(CurvTriMesh::VertexHandle vh, CurvTriMesh::HalfedgeHandle heh);
	float dihedralAngle(CurvTriMesh::HalfedgeHandle heh);
	OpenMesh::Vec3f  vertexNormalAngleWeighted(CurvTriMesh::VertexHandle vh);
	OpenMesh::Vec3f  vertexNormalSphereInscribed(CurvTriMesh::VertexHandle vh);
	OpenMesh::Vec3f  vertexNormalAreaWeighted(CurvTriMesh::VertexHandle vh);
	OpenMesh::Vec3f  vertexNormalGaussianCurvature(CurvTriMesh::VertexHandle vh);
	float angleDefect(CurvTriMesh::VertexHandle vh);
	OpenMesh::Vec3f  vertexNormalMeanCurvature(CurvTriMesh::VertexHandle vh);
	float scalarMeanCurvature(CurvTriMesh::VertexHandle vh);
	float totalAngleDefect();
	float circumcentricDualArea(CurvTriMesh::VertexHandle vh);
	float cotan(CurvTriMesh::HalfedgeHandle heh);
	PrincipalCurvatures principleCurvatures(VertexHandle vh);
};