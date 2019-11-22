#pragma once
#define GLM_ENABLE_EXPERIMENTAL
/*

Data structures for learnply

Eugene Zhang 2005

*/

#ifndef __LEARNPLY_H__
#define __LEARNPLY_H__


#include "ply.h"
#include <vector>
#include <unordered_set>
#include <map>
#include <deque>
#include <algorithm>
#include <OpenGLFiles\glm-0.9.9-a2\glm\mat4x4.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\glm.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtc\matrix_transform.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtc\type_ptr.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtx\rotate_vector.hpp>

/*

Data structure for I/O of polygonal models

Eugene Zhang 2005

*/

#ifndef __LEARNPLY_IO_H__
#define __LEARNPLY_IO_H__

typedef struct Vertex_io {
	double x, y, z;
	double vx, vy, vz;
	double s;
	void* other_props;       /* other properties */
} Vertex_io;

typedef struct Face_io {
	unsigned char nverts;    /* number of vertex indices in list */
	int* verts;              /* vertex index list */
	void* other_props;       /* other properties */
} Face_io;

static char* elem_names[] = { /* list of the kinds of elements in the user's object */
	"vertex", "face"
};

static PlyProperty vert_props[] = { /* list of property information for a vertex */
	{"x", Float64, Float64, offsetof(Vertex_io,x), 0, 0, 0, 0},
	{"y", Float64, Float64, offsetof(Vertex_io,y), 0, 0, 0, 0},
	{"z", Float64, Float64, offsetof(Vertex_io,z), 0, 0, 0, 0},
	{"vx", Float64, Float64, offsetof(Vertex_io,vx), 0, 0, 0, 0},
	{"vy", Float64, Float64, offsetof(Vertex_io,vy), 0, 0, 0, 0},
	{"vz", Float64, Float64, offsetof(Vertex_io,vz), 0, 0, 0, 0},
	{"s", Float64, Float64, offsetof(Vertex_io,s), 0, 0, 0, 0},
};

static PlyProperty face_props[] = { /* list of property information for a face */
	{"vertex_indices", Int32, Int32, offsetof(Face_io,verts), 1, Uint8, Uint8, offsetof(Face_io,nverts)},
};

#endif /* __LEARNPLY_IO_H__ */



typedef glm::vec3 vec3;
const double EPS = 1.0e-6;
const double PI = 3.1415926535898;

/* forward declarations */
class Quad;
class Polyhedron;
class Isolines;

class Vertex {
public:
	double x, y, z;
	double vx, vy, vz;
	double scalar = 0;
	double _v_length = 0;
	inline double p_length() { return sqrt(x * x + y * y + z * z); }
	inline double v_length() { return sqrt(vx * vx + vy * vy + vz * vz); }
	Vertex* left_neighbor;
	Vertex* right_neighbor;
	Vertex* up_neighbor;
	Vertex* down_neighbor;
	double R = 0, G = 0, B = 0;
	bool iso_plot;
	int index;
	std::vector<int> neighbors;
	int nquads;
	Quad** quads;
	int max_quads;
	int type;
	vec3 normal;
	void* other_props;
	bool is_scalar_local_min = false;
	bool is_scalar_local_max = false;
public:
	Vertex(double xx, double yy, double zz) { x = xx; y = yy; z = zz; left_neighbor = right_neighbor = up_neighbor = down_neighbor = nullptr; }
	Vertex(double xx, double yy, double zz, double vl, double s, int t) {
		x = xx; y = yy; z = zz; _v_length = vl; scalar = s; type = t; left_neighbor = right_neighbor = up_neighbor = down_neighbor = nullptr;
	}
};

class Edge {
public:
	int index;
	Vertex* verts[2];
	int nquads;
	Quad** quads;
	double length;
	bool iso_plot;
};

class Isolines {
public:

};

class Quad {
public:
	int index;
	int nverts;
	unsigned int type;
	Vertex* verts[4];
	Edge* edges[4];
	double angle[4];
	float area;
	unsigned int quad_type_isoline;

	vec3 normal;
	void* other_props;
};



class Polyhedron {
public:
	int polyid;
	vec3 min, max;
	double min_scalar;
	double min_vector_length;
	double max_scalar;
	double max_vector_length;
	vec3 min_vx, max_vx;
	float scalar_variance;
	float v_variance;
	float scalar_mean;
	float v_mean;
	int index;

	Quad** qlist;  /* list of quads */
	int nquads;
	int max_quads;

	Vertex** vlist;    /* list of vertices */
	int nverts;
	int max_verts;

	Edge** elist;      /* list of edges */
	int nedges;
	int max_edges;

	vec3 center;

	std::vector<int> singularity_quad_ids;
	double radius;
	double area;

	int seed;

	PlyOtherProp* vert_other, *face_other;

	void average_normals();
	std::vector < std::pair<vec3, vec3> > get_ordered_quad(int quad_id);
	void create_edge(Vertex*, Vertex*);
	void create_edges();
	int face_to_vertex_ref(Quad*, Vertex*);
	void order_vertex_to_quad_ptrs(Vertex*);
	void vertex_to_quad_ptrs();
	Quad* find_common_edge(Quad*, Vertex*, Vertex*);
	Quad* other_quad(Edge*, Quad*);
	void calc_bounding_sphere();
	void calc_face_normals_and_area();
	void calc_edge_length();
	Polyhedron();
	Polyhedron(FILE*, int n);
	void write_file(FILE*);

	void create_pointers();
	/***********************/
	//   p2--p1			   //
	//   |    |	Interpolate//
	//   p3--p0			   //
	/***********************/
	vec3 interpolate(vec3 &p0, vec3 &p1, double s);
	vec3 interpolate(vec3& p0, vec3& p1, double v0s, double v1s, double s);
	/***********************/
	// initialization and finalization
	void initialize();
	void finalize();
};


#endif /* __LEARNPLY_H__ */

