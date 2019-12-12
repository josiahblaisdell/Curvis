#pragma once
#include "typdef.h"
class CurvTriMesh;
class CurvComputer
{
public:
	  CurvComputer();
	 ~CurvComputer();

	 bool Execute(CurvTriMesh* mesh);
private:
	 bool CalaFaceProps(CurvTriMesh* mesh);
	 float cotan(TriMesh::Point u, TriMesh::Point v);
	 bool CalaVertexProps (CurvTriMesh* mesh);
	 bool CalaGaussianCurvature(CurvTriMesh* mesh);
	 bool CalaMeanCurvature(CurvTriMesh* mesh);
	 bool CalaCurvatureDirection(CurvTriMesh* mesh);

	 bool SmoothGaussian(CurvTriMesh* mesh);
	 bool SmoothMeanNormal(CurvTriMesh* mesh);
	 bool SmoothCurvatureDirection(CurvTriMesh* mesh);

	 float Heron(TriMesh::Point& p0, TriMesh::Point& p1, TriMesh::Point& p2);
	 OpenMesh::Vec3f FaceNormal(TriMesh::Point& p0, TriMesh::Point& p1, TriMesh::Point& p2);

	 static void linsolve(float m11, float m12, float b1,
						  float m21, float m22, float b2,
						  float* x1, float* x2);
	 /* major eigenvector of[a b; b c] */
	 void eigenvector(float a, float b, float c, OpenMesh::Vec2f& e);
};

