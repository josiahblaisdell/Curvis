#pragma once
#include "typdef.h"
#include <glm\glm.hpp>
#include <vector>

class CurvTriMesh: public TriMesh
{
public:
	typedef std::pair<float, float> PrincipalCurvatures;
	typedef std::pair<OpenMesh::Vec3f, OpenMesh::Vec3f> CurvatureDirection;
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
public:
	 CurvTriMesh();
	~CurvTriMesh();
	/*Compute Curvature*/
	bool Boot();

	/*Load */
	void Init();
private:
	friend class CurvComputer;
	bool  m_Computed;
	inline void  SetArea(const TriMesh::FaceHandle& fh, float value)        { this->property(m_Area, fh) = value; }
	inline void  SetMixedArea(const TriMesh::VertexHandle& vh, float value) { this->property(m_MixedArea, vh) = value; }
	inline void  SetGaussain(const TriMesh::VertexHandle& vh, float value)  { this->property(m_Gaussain, vh) = value; }
	inline void  SetMean(const TriMesh::VertexHandle& vh, OpenMesh::Vec3f& value) { this->property(m_Mean, vh) = value; }
	inline void  SetCurveDirection(const TriMesh::VertexHandle& vh, CurvatureDirection& value) { this->property(m_CurvatureDir, vh) = value; }
public:
	
	inline float GetArea(const TriMesh::FaceHandle& fh) { return this->property(m_Area, fh); }

	inline float GetMixedArea(const TriMesh::VertexHandle& vh) { return this->property(m_MixedArea, vh); }

	inline float GetGaussain(const TriMesh::VertexHandle& vh) { return this->property(m_Gaussain, vh); }

	inline OpenMesh::Vec3f GetMean(const TriMesh::VertexHandle& vh) { return this->property(m_Mean, vh); }

	inline CurvatureDirection GetCurvatureDirection(const TriMesh::VertexHandle& vh) { return this->property(m_CurvatureDir, vh); }

	inline PrincipalCurvatures  GetPrincipalCurvatures(const TriMesh::VertexHandle& vh);
public:

	bool GenVertexBuffer(float* out_buffer, int* out_size);

	bool GenVertexNormalBuffer(float* out_buffer, int* out_size);

	bool GenVertexNormalBuffer(std::vector<glm::vec3>* out_buffer, unsigned long long * out_size);

	bool GenVertexMajorDirectionBuffer(float* out_buffer, int* out_size);

	bool GenVertexMinorDirectionBuffer(float* out_buffer, int* out_size);
};