#include "CurvComputer.h"
#include "CurvTriMesh.h"

CurvComputer::CurvComputer()  { }

CurvComputer::~CurvComputer() { }

bool CurvComputer::Execute(CurvTriMesh* mesh)
{
	bool flag;
	//Step 1: Calculate Area and Normal of Faces
	flag = CalaFaceProps(mesh);
	if (!flag) { return false; }
	//Step 2: Calculate Area and Normal of Faces
	flag = CalaVertexProps(mesh);
	if (!flag) { return false; }
	//Step 3: Calculate Gussain Curvature
	flag = CalaGaussianCurvature(mesh);
	if (!flag) { return false; }
	//for (int i = 0; i < 3; i++) { SmoothGaussian(mesh); }
	//Step 4: Calculate Mean Curvature Normal
	flag = CalaMeanCurvature(mesh);
	if (!flag) { return false; }
	for (int i = 0; i < 1; i++) { SmoothMeanNormal(mesh); }
	//Step 5: Calculate Principal Curvature Direction
	flag = CalaCurvatureDirection(mesh);
	if (!flag) { return false; }
	for (int i = 0; i < 10; i++) { SmoothCurvatureDirection(mesh); }
	mesh->m_Computed = true;
	return true;
}

bool CurvComputer::CalaFaceProps(CurvTriMesh* mesh)
{
	for (CurvTriMesh::FaceIter f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it)
	{
		CurvTriMesh::Point pt[3]; int i = 0;
		for (CurvTriMesh::FaceVertexIter fv_it = mesh->fv_begin(f_it); fv_it != mesh->fv_end(f_it); fv_it++)
		{
			pt[i++] = mesh->point(fv_it);
		}
		float area = Heron(pt[0], pt[1], pt[2]);
		mesh->SetArea(f_it, area);
		OpenMesh::Vec3f norm = FaceNormal(pt[0], pt[1], pt[2]);
		mesh->set_normal(f_it, norm);
	}
	return true;
}

bool CurvComputer::CalaVertexProps(CurvTriMesh* mesh)
{
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f norm(0.0f);
		float area = 0.0f;
		for (CurvTriMesh::VertexFaceIter vf_it = mesh->vf_begin(v_it); vf_it != mesh->vf_end(v_it); ++vf_it)
		{
			norm += mesh->normal (vf_it);
			area += mesh->GetArea(vf_it)/3.0f;
		}
		norm.normalize();
		mesh->set_normal(v_it, norm);
		mesh->SetMixedArea(v_it, area);
	}
	return true;
}

bool CurvComputer::CalaGaussianCurvature(CurvTriMesh* mesh)
{
	static const float pi2 = 6.28318530718f;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		CurvTriMesh::Point p0 = mesh->point(v_it);
		float total_theta = 0.0f;
		// Iterate over all outgoing halfedges...
		for (CurvTriMesh::VertexOHalfedgeIter voh_it = mesh->voh_iter(v_it); voh_it; ++voh_it)
		{
			if (mesh->is_boundary(voh_it))
			{
				std::cout << "[CalaGaussianCurvature] It is not a close mesh" << std::endl;
				return false;
			}
			CurvTriMesh::VertexHandle v1 = mesh->to_vertex_handle(voh_it);
			CurvTriMesh::Point p1 = mesh->point(v1);
			CurvTriMesh::VertexHandle v2 = mesh->to_vertex_handle(mesh->next_halfedge_handle(voh_it));
			CurvTriMesh::Point p2 = mesh->point(v2);
			OpenMesh::Vec3f a = (p1 - p0).normalize();
			OpenMesh::Vec3f b = (p2 - p0).normalize();
			total_theta += acos(OpenMesh::dot(a, b));
		}
		float mixed_area = mesh->GetMixedArea(v_it);
		if (mixed_area != 0.0f)
		{
			float k_g = (pi2 - total_theta) / mixed_area;
			mesh->SetGaussain(v_it, k_g);
		}
		else
		{
			std::cout << "[CalaGaussianCurvature] It is not an non-manifold mesh" << std::endl;
			return false;
		}
	}
	return true;
}

bool CurvComputer::CalaMeanCurvature(CurvTriMesh* mesh)
{
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		CurvTriMesh::Point pi = mesh->point(v_it);
		OpenMesh::Vec3f mean(0.0f, 0.0f, 0.0f);
		// Iterate over all outgoing halfedges...
		//       vi
		//    /  ||  \
		//  v2   ||  v1
		//    \  ||  /  
		//       vj
		for (CurvTriMesh::VertexOHalfedgeIter voh_it = mesh->voh_iter(v_it); voh_it; ++voh_it)
		{
			if (mesh->is_boundary(voh_it))
			{
				std::cout << "[CalaMeanCurvature] It is not a close mesh" << std::endl;
				return false;
			}
			CurvTriMesh::VertexHandle vj = mesh->to_vertex_handle(voh_it);
			CurvTriMesh::Point pj = mesh->point(vj);

			CurvTriMesh::VertexHandle v1 = mesh->to_vertex_handle(mesh->next_halfedge_handle(voh_it));
			CurvTriMesh::Point p1 = mesh->point(v1);
			OpenMesh::Vec3f a = (pi - p1).normalize();
			OpenMesh::Vec3f b = (pj - p1).normalize();
			float alpha = acos(OpenMesh::dot(a, b));
			float tanA = tan(alpha);

			CurvTriMesh::HalfedgeHandle opHEH = mesh->opposite_halfedge_handle(voh_it);
			CurvTriMesh::VertexHandle v2 = mesh->to_vertex_handle(mesh->next_halfedge_handle(opHEH));
			CurvTriMesh::Point p2 = mesh->point(v2);
			OpenMesh::Vec3f c = (pi - p2).normalize();
			OpenMesh::Vec3f d = (pj - p2).normalize();
			float beta = acos(OpenMesh::dot(c, d));
			float tanB = tan(beta);

			if (tanA == 0.0f || tanB == 0.0f)
			{
				std::cout << "[CalaMeanCurvature] It is not an non-manifold mesh" << std::endl;
				return false;
			}
			mean += (1.0f / tanA + 1.0f / tanB) * (pj - pi);
		}
		float mixed_area = mesh->GetMixedArea(v_it);
		if (mixed_area != 0.0f)
		{
			mean *= 0.5f / mixed_area;
			mesh->SetMean(v_it, mean);
		}
		else
		{
			std::cout << "[CalaMeanCurvature] It is not an non-manifold mesh" << std::endl;
			return false;
		}
	}
	return true;
}

bool CurvComputer::CalaCurvatureDirection(CurvTriMesh* mesh)
{
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f normal = mesh->GetMeanCurvatureNormal(v_it);
		float kappa_h_2 = normal.norm();
		if (kappa_h_2 == 0.0f)
		{ normal = mesh->normal(v_it); }
		else 
		{ normal.normalize(); }
		/* construct a basis from Normal: */
	    /* set basis1 to any component not the largest of N */
		OpenMesh::Vec3f basis1(0.0f), basis2(0.0f);
		if (fabs(normal[0]) > fabs(normal[1]))
		{ basis1[1] = 1.0f; }
		else
		{ basis1[0] = 1.0f; }

		/* make basis2 orthogonal to N */
		basis2 = OpenMesh::cross(normal, basis1);
		basis2.normalize();

		/* make basis1 orthogonal to N and basis2 */
		basis1 = OpenMesh::cross(normal, basis2);
		basis1.normalize();

		std::vector<float> weights, kappas, d1s, d2s;
		float aterm_da = 0.0f, bterm_da = 0.0f, cterm_da = 0.0f, const_da = 0.0f;
		float aterm_db = 0.0f, bterm_db = 0.0f, cterm_db = 0.0f, const_db = 0.0f;
		// Iterate over all outgoing halfedges...
		//       vi
		//    /  ||  \
		//  v2   ||  v1
		//    \  ||  /  
		//       vj
		CurvTriMesh::Point pi = mesh->point(v_it);
		for (CurvTriMesh::VertexOHalfedgeIter voh_it = mesh->voh_iter(*v_it); voh_it; ++voh_it)
		{
			if (mesh->is_boundary(voh_it))
			{
				std::cout << "[CalaMeanCurvature] It is not a close mesh" << std::endl;
				return false;
			}
			CurvTriMesh::VertexHandle vj = mesh->to_vertex_handle(voh_it);
			CurvTriMesh::Point pj = mesh->point(vj);

			CurvTriMesh::VertexHandle v1 = mesh->to_vertex_handle(mesh->next_halfedge_handle(voh_it));
			CurvTriMesh::Point p1 = mesh->point(v1);
			OpenMesh::Vec3f v_1i = (pi - p1).normalize();
			OpenMesh::Vec3f v_1j = (pj - p1).normalize();
			float alpha = acos(OpenMesh::dot(v_1i, v_1j));
			float tanA  = tan(alpha);

			CurvTriMesh::HalfedgeHandle opHEH = mesh->opposite_halfedge_handle(voh_it);
			CurvTriMesh::VertexHandle v2 = mesh->to_vertex_handle(mesh->next_halfedge_handle(opHEH));
			CurvTriMesh::Point p2 = mesh->point(v2);
			OpenMesh::Vec3f v_2i = (pi - p2).normalize();
			OpenMesh::Vec3f v_2j = (pj - p2).normalize();
			float beta = acos(OpenMesh::dot(v_2i, v_2j));
			float tanB = tan(beta);

			if (tanA == 0.0f || tanB == 0.0f)
			{
				std::cout << "[CalaMeanCurvature] It is not an non-manifold mesh" << std::endl;
				return false;
			}

			/* section 5.2 - kappa.  */
			OpenMesh::Vec3f v_ij = (pj - pi);
			float kappa  = 2.0 * OpenMesh::dot(v_ij, normal) / v_ij.sqrnorm();
			/* section 5.2 - weight. */
			float weight = 0.0f;
			float mixed_area = mesh->GetMixedArea(v_it);
			if (mixed_area != 0.0f)
			{
				weight = 0.125f / mixed_area * (1.0f / tanA + 1.0f / tanB) * v_ij.sqrnorm();
			}
			/* section 5.3 - d. */
			OpenMesh::Vec3f d = v_ij - OpenMesh::dot(v_ij, normal) * normal;
			d.normalize();

			/* not explicit in the paper, but necessary.  Move d to 2D basis. */
			float d1 = OpenMesh::dot(d, basis1);
			float d2 = OpenMesh::dot(d, basis2);

			/* Finally, update the linear equations */
			aterm_da += weight * d1 * d1 * d1 * d1;
			bterm_da += weight * d1 * d1 * 2 * d1 * d2;
			cterm_da += weight * d1 * d1 * d2 * d2;
			const_da += weight * d1 * d1 * (-kappa);

			aterm_db += weight * d1 * d2 * d1 * d1;
			bterm_db += weight * d1 * d2 * 2 * d1 * d2;
			cterm_db += weight * d1 * d2 * d2 * d2;
			const_db += weight * d1 * d2 * (-kappa);

			weights.push_back(weight);
			kappas.push_back(kappa);
			d1s.push_back(d1);
			d2s.push_back(d2);
		}

		/* now use the identity (Section 5.3) a + c = |Kh| = 2 * kappa_h */
		aterm_da -= cterm_da;
		const_da += cterm_da * kappa_h_2;

		aterm_db -= cterm_db;
		const_db += cterm_db * kappa_h_2;

		/*[a, b; b, c]*/
		float a = 0.0f, b = 0.0f, c = 0.0f;
		/*Major Eigenvector*/
		OpenMesh::Vec2f eig;
		/* check for solvability of the linear system */
		if (((aterm_da * bterm_db - aterm_db * bterm_da) != 0.0f) &&
			((const_da != 0.0f) || (const_db != 0.0f))) {

			linsolve(aterm_da, bterm_da, -const_da,
				     aterm_db, bterm_db, -const_db,
				     &a, &b);

			c = kappa_h_2 - a;

			eigenvector(a, b, c, eig);
		}
		else {
			/* region of v is planar */
			eig[0] = 1.0f;
			eig[1] = 0.0f;
		}

		mesh->SetTensor(v_it, CurvTriMesh::Tensor(a,b,c));

		CurvTriMesh::CurvatureDirection cd;
		cd.first[0] = eig[0] * basis1[0] + eig[1] * basis2[0];
		cd.first[1] = eig[0] * basis1[1] + eig[1] * basis2[1];
		cd.first[2] = eig[0] * basis1[2] + eig[1] * basis2[2];
		cd.first.normalize();

		/* make N,e1,e2 a right handed coordinate sytem */
		cd.second = OpenMesh::cross(cd.first, normal);
		cd.second.normalize();

		mesh->SetCurvatureDirection(v_it, cd);
	}
	return true;
}

bool CurvComputer::SmoothGaussian(CurvTriMesh* mesh)
{
	std::vector<float> gaussians;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		float count = 0.0f;
		float g = mesh->GetGaussainCurvature(v_it);
		for (CurvTriMesh::VertexVertexIter vv_it = mesh->vv_begin(v_it); vv_it != mesh->vv_end(v_it); ++vv_it)
		{
			g += mesh->GetGaussainCurvature(vv_it);
			count++;
		}
		g /= count;
		gaussians.push_back(g);
	}

	int i = 0;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it, i++)
	{
		mesh->SetGaussain(v_it, gaussians[i]);
	}
	return false;
}

bool CurvComputer::SmoothMeanNormal(CurvTriMesh* mesh)
{
	std::vector<OpenMesh::Vec3f> means;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		float count = 0.0f;
		OpenMesh::Vec3f mean = mesh->GetMeanCurvatureNormal(v_it);
		for (CurvTriMesh::VertexVertexIter vv_it = mesh->vv_begin(v_it); vv_it != mesh->vv_end(v_it); ++vv_it)
		{
			mean += mesh->GetMeanCurvatureNormal(vv_it);
			count++;
		}
		mean /= count;
		means.push_back(mean);
	}

	int i = 0;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it, i++)
	{
		mesh->SetMean(v_it, means[i]);
	}
	return false;
}

bool CurvComputer::SmoothCurvatureDirection(CurvTriMesh* mesh)
{
	std::vector<CurvTriMesh::CurvatureDirection> cds;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
	{
		OpenMesh::Vec3f major = mesh->GetCurvatureDirection(v_it).first;
		for (CurvTriMesh::VertexVertexIter vv_it = mesh->vv_begin(v_it); vv_it != mesh->vv_end(v_it); ++vv_it)
		{
			major += mesh->GetCurvatureDirection(vv_it).first;
		}
		major.normalize();

		OpenMesh::Vec3f minor = OpenMesh::cross(major, mesh->normal(v_it));
		minor.normalize();

		cds.push_back(CurvTriMesh::CurvatureDirection(major, minor));
	}

	int i = 0;
	for (CurvTriMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it, i++)
	{
		mesh->SetCurvatureDirection(v_it, cds[i]);
	}
	return false;
}

float CurvComputer::Heron(CurvTriMesh::Point& p0, CurvTriMesh::Point& p1, CurvTriMesh::Point& p2)
{
	float a = (p1 - p0).length();
	float b = (p2 - p0).length();
	float c = (p2 - p1).length();
	float s = (a + b + c) * 0.5f;
	return sqrt(s * (s -a) * (s - b) * (s - c));
}

OpenMesh::Vec3f CurvComputer::FaceNormal(CurvTriMesh::Point& p0, CurvTriMesh::Point& p1, CurvTriMesh::Point& p2)
{
	OpenMesh::Vec3f v1 = p1 - p0;
	OpenMesh::Vec3f v2 = p2 - p0;
	OpenMesh::Vec3f norm = OpenMesh::cross(v1, v2);
	return norm.normalized();
}

void CurvComputer::linsolve(float m11, float m12, float b1, float m21, float m22, float b2, float* x1, float* x2)
{
	float temp = 1.0f / (m21 * m12 - m11 * m22);
	*x1 = (m12 * b2 - m22 * b1) * temp;
	*x2 = (m11 * b2 - m21 * b1) * temp;
}

void CurvComputer::eigenvector(float a, float b, float c, OpenMesh::Vec2f& e)
{
	float theta = atan2f(2*b, a - c);
	e[0] = cos(theta * 0.5f);
	e[1] = sin(theta * 0.5f);
}