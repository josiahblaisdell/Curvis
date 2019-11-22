#pragma once
/*

Functions for learnply

Eugene Zhang, 2005
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include "ply.h"
#include "plyUtility.h"
#include "trackball.h"
#include <regex>
#include <cstdlib>

typedef glm::mat4 mat4;
typedef glm::vec4 Quaternion;
static bool test_pt(float x, float y, std::vector<std::pair<vec3, vec3>> qpts) {
	return (x > qpts[0].first.x&& y > qpts[0].first.y&& x < qpts[2].first.x && y < qpts[2].first.y);
}
/**************HW 1 - Josiah Blaisdell***************/
float min_length[100] = { FLT_MAX }; //store the min vector length in the vector field
float max_length[100] = { 0.0f };    //store the max vector length in the vector field  
bool small_folder = true; // if true use ../quadmesh_2D/small_scaler_value/arrow.ply otherwise use ../quadmesh_2D/large_scaler_value/arrow.ply
float min_s[100] = { FLT_MAX };
float max_s[100] = { 0.0f };
double scale_unit_interval(double x, double min_x, double max_x) {
	return (max_x - x) / (max_x - min_x);
}
double interp_min_val(double x, double min_x, double max_x) {
	return (x - min_x) / (max_x - min_x);
}
double interp_max_val(double x, double min_x, double max_x) {
	return (max_x - x) / (max_x - min_x);
}
std::string fn;
/****************************************************/

/**************HW 1 - Josiah Blaisdell***************/
float v_mean[100];
float s_mean[100];
float v_variance[100];
float s_variance[100];
bool test_b = true;
/****************************************************/

/**************HW 2 - Josiah Blaisdell***************/
int NUM_CONTOURS = 30;
enum CONTOURS_SWITCH
{
	CONTOURS_ON,
	CONTOURS_OFF
};
CONTOURS_SWITCH contour_switch = CONTOURS_ON;
bool contour_s_v;
double maxy = -1000;
/****************************************************/
/**************HW 4 - Josiah Blaisdell***************/
enum STREAMLINES_SWITCH
{
	STREAMLINES_ON,
	STREAMLINES_OFF
};
STREAMLINES_SWITCH streamlines_switch = STREAMLINES_ON;
/****************************************************/
static PlyFile *in_ply;

unsigned char orientation;  // 0=ccw, 1=cw

FILE *this_file;
const int win_width = 1024;
const int win_height = 1024;

double radius_factor = 0.9;

int display_mode = 0;
double error_threshold = 1.0e-13;
char reg_model_name[128];
FILE *f;
int ACSIZE = 1; // for antialiasing
int view_mode = 0;  // 0 = othogonal, 1=perspective
float s_old, t_old;
mat4 rotmat;
static mat4 rvec;

int mouse_mode = -2;  // -2=no action, -1 = down, 0 = zoom, 1 = rotate x, 2 = rotate y, 3 = tranlate x, 4 = translate y, 5 = cull near 6 = cull far
int mouse_button = -1; // -1=no button, 0=left, 1=middle, 2=right
int last_x, last_y;

double zoom = 1.0;
double translation[2] = { 0, 0 };

struct jitter_struct {
	double x;
	double y;
} jitter_para;

jitter_struct ji1[1] = { {0.0, 0.0} };
jitter_struct ji16[16] = { {0.125, 0.125}, {0.375, 0.125}, {0.625, 0.125}, {0.875, 0.125},
						  {0.125, 0.375}, {0.375, 0.375}, {0.625, 0.375}, {0.875, 0.375},
						  {0.125, 0.625}, {0.375, 0.625}, {0.625, 0.625}, {0.875, 0.625},
						  {0.125, 0.875}, {0.375, 0.875}, {0.625, 0.875}, {0.875, 0.875}, };

Polyhedron *poly;
static float m;
static float v;
/******************************************************************************
Main program.
******************************************************************************/

void color_mapping(double percentage, double col[3])
{
	if (percentage == 0.0) {
		col[0] = 1.0;
		col[1] = 1.0;
		col[2] = 1.0;
	}
	else if (percentage <= 1.0 / 3) {
		col[0] = 1.0;
		col[1] = 1.0 - percentage * 3.0;
		col[2] = 1.0 - percentage * 3.0;
	}
	else if (percentage <= 2.0 / 3) {
		col[0] = 1.0;
		col[1] = percentage * 3.0 - 1.0;
		col[2] = 0.0;
	}
	else if (percentage <= 3.0 / 3) {
		col[0] = 3.0 - percentage * 3.0;
		col[1] = 1.0;
		col[2] = 0.0;
	}
	else {
		col[0] = 1.0;
		col[1] = 1.0;
		col[2] = 0.0;
	}
}

/******************************************************************************
Read in a polyhedron from a file.
******************************************************************************/

Polyhedron::Polyhedron(FILE *file, int n = 0)
{
	min_length[n] = FLT_MAX; //store the min vector length in the vector field
	max_length[n] = 0.0f;    //store the max vector length in the vector field

	int i, j;
	int elem_count;
	char *elem_name;

	/*** Read in the original PLY object ***/
	in_ply = read_ply(file);

	for (i = 0; i < in_ply->num_elem_types; i++) {

		/* prepare to read the i'th list of elements */
		elem_name = setup_element_read_ply(in_ply, i, &elem_count);

		if (equal_strings("vertex", elem_name)) {

			/* create a vertex list to hold all the vertices */
			nverts = max_verts = elem_count;
			vlist = new Vertex *[nverts];

			/* set up for getting vertex elements */

			setup_property_ply(in_ply, &vert_props[0]);
			setup_property_ply(in_ply, &vert_props[1]);
			setup_property_ply(in_ply, &vert_props[2]);
			setup_property_ply(in_ply, &vert_props[3]);
			setup_property_ply(in_ply, &vert_props[4]);
			setup_property_ply(in_ply, &vert_props[5]);
			setup_property_ply(in_ply, &vert_props[6]);

			vert_other = get_other_properties_ply(in_ply,
				offsetof(Vertex_io, other_props));

			/* grab all the vertex elements */
			for (j = 0; j < nverts; j++) {
				Vertex_io vert;
				get_element_ply(in_ply, (void*)& vert);

				/* copy info from the "vert" structure */
				vlist[j] = new Vertex(vert.x, vert.y, vert.z);
				vlist[j]->vx = vert.vx;
				vlist[j]->vy = vert.vy;
				vlist[j]->vz = vert.vz;
				vlist[j]->scalar = vert.s;
				vlist[j]->_v_length = vlist[j]->v_length();

				/**************HW 1 - Josiah Blaisdell***************/
				if (length(vec3(vert.vx, vert.vy, vert.vz)) > max_length[n]) max_length[n] = length(vec3(vert.vx, vert.vy, vert.vz));
				if (length(vec3(vert.vx, vert.vy, vert.vz)) < min_length[n]) min_length[n] = length(vec3(vert.vx, vert.vy, vert.vz));
				if (vlist[j]->scalar < min_s[n]) {
					min_s[n] = vlist[j]->scalar;
				}
				if (vlist[j]->scalar > max_s[n]) {
					max_s[n] = vlist[j]->scalar;
				}
				if (vlist[j]->y > maxy) {
					maxy = vlist[j]->y;
				}
				/****************************************************/
				vlist[j]->other_props = vert.other_props;
			}
		}
		else if (equal_strings("face", elem_name)) {

			/* create a list to hold all the face elements */
			nquads = max_quads = elem_count;
			qlist = new Quad *[nquads];

			/* set up for getting face elements */
			setup_property_ply(in_ply, &face_props[0]);
			face_other = get_other_properties_ply(in_ply, offsetof(Face_io, other_props));

			/* grab all the face elements */
			for (j = 0; j < elem_count; j++) {
				Face_io face;
				get_element_ply(in_ply, (void *)&face);

				if (face.nverts != 4) {
					fprintf(stderr, "Face has %d vertices (should be four).\n",
						face.nverts);
					exit(-1);
				}

				/* copy info from the "face" structure */
				qlist[j] = new Quad;
				qlist[j]->nverts = 4;
				qlist[j]->verts[0] = (Vertex *)face.verts[0];
				qlist[j]->verts[1] = (Vertex *)face.verts[1];
				qlist[j]->verts[2] = (Vertex *)face.verts[2];
				qlist[j]->verts[3] = (Vertex *)face.verts[3];
				qlist[j]->other_props = face.other_props;
			}
		}
		else
			get_other_element_ply(in_ply);
	}
	min_scalar = min_s[n];
	max_scalar = max_s[n];
	min_vector_length = min_length[n];
	max_vector_length = max_length[n];

	/* close the file */
	close_ply(in_ply);

	/* fix up vertex pointers in quads */
	for (i = 0; i < nquads; i++) {
		qlist[i]->verts[0] = vlist[(int)qlist[i]->verts[0]];
		qlist[i]->verts[1] = vlist[(int)qlist[i]->verts[1]];
		qlist[i]->verts[2] = vlist[(int)qlist[i]->verts[2]];
		qlist[i]->verts[3] = vlist[(int)qlist[i]->verts[3]];
	}

	/* get rid of quads that use the same vertex more than once */

	for (i = nquads - 1; i >= 0; i--) {

		Quad *quad = qlist[i];
		Vertex *v0 = quad->verts[0];
		Vertex *v1 = quad->verts[1];
		Vertex *v2 = quad->verts[2];
		Vertex *v3 = quad->verts[3];

		if (v0 == v1 || v1 == v2 || v2 == v3 || v3 == v0) {
			free(qlist[i]);
			nquads--;
			qlist[i] = qlist[nquads];
		}
	}
}


/******************************************************************************
Write out a polyhedron to a file.
******************************************************************************/
void Polyhedron::write_file(FILE *file)
{
	int i;
	PlyFile *ply;
	char **elist;
	int num_elem_types;

	/*** Write out the transformed PLY object ***/

	elist = get_element_list_ply(in_ply, &num_elem_types);
	ply = write_ply(file, num_elem_types, elist, in_ply->file_type);

	/* describe what properties go into the vertex elements */

	describe_element_ply(ply, "vertex", nverts);
	describe_property_ply(ply, &vert_props[0]);
	describe_property_ply(ply, &vert_props[1]);
	describe_property_ply(ply, &vert_props[2]);
	//  describe_other_properties_ply (ply, vert_other, offsetof(Vertex_io,other_props));

	describe_element_ply(ply, "face", nquads);
	describe_property_ply(ply, &face_props[0]);

	//  describe_other_properties_ply (ply, face_other,
	//                                offsetof(Face_io,other_props));

	//  describe_other_elements_ply (ply, in_ply->other_elems);

	copy_comments_ply(ply, in_ply);
	char mm[1024];
	sprintf(mm, "modified by learnply");
	//  append_comment_ply (ply, "modified by simvizply %f");
	append_comment_ply(ply, mm);
	copy_obj_info_ply(ply, in_ply);

	header_complete_ply(ply);

	/* set up and write the vertex elements */
	put_element_setup_ply(ply, "vertex");
	for (i = 0; i < nverts; i++) {
		Vertex_io vert;

		/* copy info to the "vert" structure */
		vert.x = vlist[i]->x;
		vert.y = vlist[i]->y;
		vert.z = vlist[i]->z;
		vert.other_props = vlist[i]->other_props;

		put_element_ply(ply, (void *)&vert);
	}

	/* index all the vertices */
	for (i = 0; i < nverts; i++)
		vlist[i]->index = i;

	/* set up and write the face elements */
	put_element_setup_ply(ply, "face");

	Face_io face;
	face.verts = new int[4];

	for (i = 0; i < nquads; i++) {

		/* copy info to the "face" structure */
		face.nverts = 4;
		face.verts[0] = qlist[i]->verts[0]->index;
		face.verts[1] = qlist[i]->verts[1]->index;
		face.verts[2] = qlist[i]->verts[2]->index;
		face.verts[3] = qlist[i]->verts[3]->index;
		face.other_props = qlist[i]->other_props;

		put_element_ply(ply, (void *)&face);
	}
	put_other_elements_ply(ply);

	close_ply(ply);
	free_ply(ply);
}

void Polyhedron::initialize() {
	vec3 v1, v2;

	create_pointers();
	calc_edge_length();
	seed = -1;
}

void Polyhedron::finalize() {
	int i;

	for (i = 0; i < nquads; i++) {
		if (qlist[i]->other_props) {
			free((char*)qlist[i]->other_props);
		}
		if (qlist[i]) {
			free(qlist[i]);
		}
	}
	for (i = 0; i < nedges; i++) {
		if (elist[i]) {
			free(elist[i]->quads);
			free(elist[i]);
		}
	}
	for (i = 0; i < nverts - 1; i++) {
		if (vlist[i]) {
			free(vlist[i]->quads);
			free(vlist[i]->other_props);
			free(vlist[i]);
		}
	}

	if (qlist) free(qlist);
	if (elist) free(elist);
	if (vlist) free(vlist);
	if (!vert_other)
		free(vert_other);
	if (!face_other)
		free(face_other);
}

/******************************************************************************
Find out if there is another face that shares an edge with a given face.

Entry:
  f1    - face that we're looking to share with
  v1,v2 - two vertices of f1 that define edge

Exit:
  return the matching face, or NULL if there is no such face
******************************************************************************/

Quad *Polyhedron::find_common_edge(Quad *f1, Vertex *v1, Vertex *v2)
{
	int i, j;
	Quad *f2;
	Quad *adjacent = NULL;

	/* look through all faces of the first vertex */

	for (i = 0; i < v1->nquads; i++) {
		f2 = v1->quads[i];
		if (f2 == f1)
			continue;
		/* examine the vertices of the face for a match with the second vertex */
		for (j = 0; j < f2->nverts; j++) {

			/* look for a match */
			if (f2->verts[j] == v2) {

#if 0
				/* watch out for triple edges */

				if (adjacent != NULL) {

					fprintf(stderr, "model has triple edges\n");

					fprintf(stderr, "face 1: ");
					for (k = 0; k < f1->nverts; k++)
						fprintf(stderr, "%d ", f1->iverts[k]);
					fprintf(stderr, "\nface 2: ");
					for (k = 0; k < f2->nverts; k++)
						fprintf(stderr, "%d ", f2->iverts[k]);
					fprintf(stderr, "\nface 3: ");
					for (k = 0; k < adjacent->nverts; k++)
						fprintf(stderr, "%d ", adjacent->iverts[k]);
					fprintf(stderr, "\n");

				}

				/* if we've got a match, remember this face */
				adjacent = f2;
#endif

#if 1
				/* if we've got a match, return this face */
				return (f2);
#endif

			}
		}
	}

	return (adjacent);
}


/******************************************************************************
Create an edge.

Entry:
  v1,v2 - two vertices of f1 that define edge
******************************************************************************/

void Polyhedron::create_edge(Vertex *v1, Vertex *v2)
{
	int i, j;
	Quad *f;

	/* make sure there is enough room for a new edge */

	if (nedges >= max_edges) {

		max_edges += 100;
		Edge **list = new Edge *[max_edges];

		/* copy the old list to the new one */
		for (i = 0; i < nedges; i++)
			list[i] = elist[i];

		/* replace list */
		free(elist);
		elist = list;
	}

	/* create the edge */

	elist[nedges] = new Edge;
	Edge *e = elist[nedges];
	e->index = nedges;
	e->verts[0] = v1;
	e->verts[1] = v2;
	nedges++;

	/* count all quads that will share the edge, and do this */
	/* by looking through all faces of the first vertex */

	e->nquads = 0;

	for (i = 0; i < v1->nquads; i++) {
		f = v1->quads[i];
		/* examine the vertices of the face for a match with the second vertex */
		for (j = 0; j < 4; j++) {
			/* look for a match */
			if (f->verts[j] == v2) {
				e->nquads++;
				break;
			}
		}
	}

	/* make room for the face pointers (at least two) */
	if (e->nquads < 2)
		e->quads = new Quad *[2];
	else
		e->quads = new Quad *[e->nquads];

	/* create pointers from edges to faces and vice-versa */

	e->nquads = 0; /* start this out at zero again for creating ptrs to quads */

	for (i = 0; i < v1->nquads; i++) {

		f = v1->quads[i];

		/* examine the vertices of the face for a match with the second vertex */
		for (j = 0; j < 4; j++)
			if (f->verts[j] == v2) {

				e->quads[e->nquads] = f;
				e->nquads++;

				if (f->verts[(j + 1) % 4] == v1)
					f->edges[j] = e;
				else if (f->verts[(j + 2) % 4] == v1)
					f->edges[(j + 2) % 4] = e;
				else if (f->verts[(j + 3) % 4] == v1)
					f->edges[(j + 3) % 4] = e;
				else {
					fprintf(stderr, "Non-recoverable inconsistancy in create_edge()\n");
					exit(-1);
				}

				break;  /* we'll only find one instance of v2 */
			}

	}
}


/******************************************************************************
Create edges.
******************************************************************************/

void Polyhedron::create_edges()
{
	int i, j;
	Quad *f;
	Vertex *v1, *v2;
	double count = 0;

	/* count up how many edges we may require */

	for (i = 0; i < nquads; i++) {
		f = qlist[i];
		for (j = 0; j < f->nverts; j++) {
			v1 = f->verts[j];
			v2 = f->verts[(j + 1) % f->nverts];
			Quad *result = find_common_edge(f, v1, v2);
			if (result)
				count += 0.5;
			else
				count += 1;
		}
	}

	/*
	printf ("counted %f edges\n", count);
	*/

	/* create space for edge list */

	max_edges = (int)(count + 10);  /* leave some room for expansion */
	elist = new Edge *[max_edges];
	nedges = 0;

	/* zero out all the pointers from faces to edges */

	for (i = 0; i < nquads; i++)
		for (j = 0; j < 4; j++)
			qlist[i]->edges[j] = NULL;

	/* create all the edges by examining all the quads */

	for (i = 0; i < nquads; i++) {
		f = qlist[i];
		for (j = 0; j < 4; j++) {
			/* skip over edges that we've already created */
			if (f->edges[j])
				continue;
			v1 = f->verts[j]->x > f->verts[(j + 1) % f->nverts]->x ? f->verts[j] : f->verts[(j + 1) % f->nverts];
			v2 = f->verts[j]->x > f->verts[(j + 1) % f->nverts]->x ? f->verts[(j + 1) % f->nverts] : f->verts[j];
			//v2   v1
			//o-----o
			v1->neighbors.push_back(v2->index);
			v2->neighbors.push_back(v1->index);
			if (v1->x > v2->x) {
				if (v1->left_neighbor == nullptr) {
					v1->left_neighbor = v2;
				}
				else {
					if (abs(v2->x - v1->x) > abs(v1->left_neighbor->x - v1->x)) {
						v1->left_neighbor = v2;
					}
				}
				if (v2->right_neighbor == nullptr) {
					v2->right_neighbor = v1;
				}
				else {
					if (abs(v2->x - v1->x) > abs(v2->right_neighbor->x - v2->x)) {
						v2->right_neighbor = v1;
					}
				}
			}
			if (v1->x < v2->x) {
				if (v1->right_neighbor == nullptr) {
					v1->right_neighbor = v2;
				}
				else {
					if (abs(v2->x - v1->x) > abs(v1->right_neighbor->x - v1->x)) {
						v1->right_neighbor = v2;
					}
				}
				if (v2->left_neighbor == nullptr) {
					v2->left_neighbor = v1;
				}
				else {
					if (abs(v2->x - v1->x) > abs(v2->left_neighbor->x - v2->x)) {
						v2->left_neighbor = v1;
					}
				}
			}
			if (v1->y < v2->y) {
				if (v1->up_neighbor == nullptr) {
					v1->up_neighbor = v2;
				}
				else {
					if (abs(v2->y - v1->y) > abs(v1->up_neighbor->y - v1->y)) {
						v1->up_neighbor = v2;
					}
				}
				if (v2->down_neighbor == nullptr) {
					v2->down_neighbor = v1;
				}
				else {
					if (abs(v2->y - v1->y) > abs(v2->down_neighbor->y - v2->y)) {
						v2->down_neighbor = v1;
					}
				}
			}
			if (v1->y > v2->y) {
				if (v1->down_neighbor == nullptr) {
					v1->down_neighbor = v2;
				}
				else {
					if (abs(v2->y - v1->y) > abs(v1->down_neighbor->y - v1->y)) {
						v1->down_neighbor = v2;
					}
				}
				if (v2->up_neighbor == nullptr) {
					v2->up_neighbor = v1;
				}
				else {
					if (abs(v2->y - v1->y) > abs(v2->up_neighbor->y - v2->y)) {
						v2->up_neighbor = v1;
					}
				}
			}
			create_edge(v1, v2);
		}
	}
}


/******************************************************************************
Create pointers from vertices to faces.
******************************************************************************/

void Polyhedron::vertex_to_quad_ptrs()
{
	int i, j;
	Quad *f;
	Vertex *v;

	/* zero the count of number of pointers to faces */

	for (i = 0; i < nverts; i++)
		vlist[i]->max_quads = 0;

	/* first just count all the face pointers needed for each vertex */

	for (i = 0; i < nquads; i++) {
		f = qlist[i];
		for (j = 0; j < f->nverts; j++)
			f->verts[j]->max_quads++;
	}

	/* allocate memory for face pointers of vertices */

	for (i = 0; i < nverts; i++) {
		vlist[i]->quads = (Quad **)
			malloc(sizeof(Quad *) * vlist[i]->max_quads);
		vlist[i]->nquads = 0;
	}

	/* now actually create the face pointers */

	for (i = 0; i < nquads; i++) {
		f = qlist[i];
		for (j = 0; j < f->nverts; j++) {
			v = f->verts[j];
			v->quads[v->nquads] = f;
			v->nquads++;
		}
	}
}


/******************************************************************************
Find the other quad that is incident on an edge, or NULL if there is
no other.
******************************************************************************/

Quad *Polyhedron::other_quad(Edge *edge, Quad *quad)
{
	/* search for any other quad */
	for (int i = 0; i < edge->nquads; i++)
		if (edge->quads[i] != quad)
			return (edge->quads[i]);

	/* there is no such other quad if we get here */
	return (NULL);
}


/******************************************************************************
Order the pointers to faces that are around a given vertex.

Entry:
  v - vertex whose face list is to be ordered
******************************************************************************/

void Polyhedron::order_vertex_to_quad_ptrs(Vertex *v)
{
	int i, j;
	Quad *f;
	Quad *fnext;
	int nf;
	int vindex;
	int boundary;
	int count;

	nf = v->nquads;
	f = v->quads[0];

	/* go backwards (clockwise) around faces that surround a vertex */
	/* to find out if we reach a boundary */

	boundary = 0;

	for (i = 1; i <= nf; i++) {

		/* find reference to v in f */
		vindex = -1;
		for (j = 0; j < f->nverts; j++)
			if (f->verts[j] == v) {
				vindex = j;
				break;
			}

		/* error check */
		if (vindex == -1) {
			fprintf(stderr, "can't find vertex #1\n");
			exit(-1);
		}

		/* corresponding face is the previous one around v */
		fnext = other_quad(f->edges[vindex], f);

		/* see if we've reached a boundary, and if so then place the */
		/* current face in the first position of the vertice's face list */

		if (fnext == NULL) {
			/* find reference to f in v */
			for (j = 0; j < v->nquads; j++)
				if (v->quads[j] == f) {
					v->quads[j] = v->quads[0];
					v->quads[0] = f;
					break;
				}
			boundary = 1;
			break;
		}

		f = fnext;
	}

	/* now walk around the faces in the forward direction and place */
	/* them in order */

	f = v->quads[0];
	count = 0;

	for (i = 1; i < nf; i++) {

		/* find reference to vertex in f */
		vindex = -1;
		for (j = 0; j < f->nverts; j++)
			if (f->verts[(j + 1) % f->nverts] == v) {
				vindex = j;
				break;
			}

		/* error check */
		if (vindex == -1) {
			fprintf(stderr, "can't find vertex #2\n");
			exit(-1);
		}

		/* corresponding face is next one around v */
		fnext = other_quad(f->edges[vindex], f);

		/* break out of loop if we've reached a boundary */
		count = i;
		if (fnext == NULL) {
			break;
		}

		/* swap the next face into its proper place in the face list */
		for (j = 0; j < v->nquads; j++)
			if (v->quads[j] == fnext) {
				v->quads[j] = v->quads[i];
				v->quads[i] = fnext;
				break;
			}

		f = fnext;
	}
}


/******************************************************************************
Find the index to a given vertex in the list of vertices of a given face.

Entry:
  f - face whose vertex list is to be searched
  v - vertex to return reference to

Exit:
  returns index in face's list, or -1 if vertex not found
******************************************************************************/

int Polyhedron::face_to_vertex_ref(Quad *f, Vertex *v)
{
	int j;
	int vindex = -1;

	for (j = 0; j < f->nverts; j++)
		if (f->verts[j] == v) {
			vindex = j;
			break;
		}

	return (vindex);
}

/******************************************************************************
Create various face and vertex pointers.
******************************************************************************/

void Polyhedron::create_pointers()
{
	int i;

	/* index the vertices and quads */

	for (i = 0; i < nverts; i++)
		vlist[i]->index = i;

	for (i = 0; i < nquads; i++)
		qlist[i]->index = i;

	/* create pointers from vertices to quads */
	vertex_to_quad_ptrs();

	/* make edges */
	create_edges();


	/* order the pointers from vertices to faces */
	for (i = 0; i < nverts; i++) {
		//		if (i %1000 == 0)
		//			fprintf(stderr, "ordering %d of %d vertices\n", i, nverts);
		order_vertex_to_quad_ptrs(vlist[i]);

	}
	/* index the edges */

	for (i = 0; i < nedges; i++) {
		//		if (i %1000 == 0)
		//			fprintf(stderr, "indexing %d of %d edges\n", i, nedges);
		elist[i]->index = i;
	}

}

void Polyhedron::calc_bounding_sphere()
{
	unsigned int i;

	for (i = 0; i < nverts; i++) {
		if (i == 0) {
			min = vec3(vlist[i]->x, vlist[i]->y, vlist[i]->z);
			max = vec3(vlist[i]->x, vlist[i]->y, vlist[i]->z);
		}
		else {
			if (vlist[i]->x < min[0])
				min[0] = vlist[i]->x;
			if (vlist[i]->x > max[0])
				max[0] = vlist[i]->x;
			if (vlist[i]->y < min[1])
				min[1] = vlist[i]->y;
			if (vlist[i]->y > max[1])
				max[1] = vlist[i]->y;
			if (vlist[i]->z < min[2])
				min[2] = vlist[i]->z;
			if (vlist[i]->z > max[2])
				max[2] = vlist[i]->z;
		}
	}
	center = .5f*(min + max);
	radius = length(center - min);
}

void Polyhedron::calc_edge_length()
{
	int i;
	vec3 v1, v2;

	for (i = 0; i < nedges; i++) {
		v1 = vec3(elist[i]->verts[0]->x, elist[i]->verts[0]->y, elist[i]->verts[0]->z);
		v2 = vec3(elist[i]->verts[1]->x, elist[i]->verts[1]->y, elist[i]->verts[1]->z);
		elist[i]->length = length(v1 - v2);
	}
}

void Polyhedron::calc_face_normals_and_area()
{
	unsigned int i, j;
	vec3 v0, v1, v2, v3;
	Quad *temp_q;
	double edge_length[4];

	area = 0.0;
	for (i = 0; i < nquads; i++) {
		for (j = 0; j < 4; j++)
			edge_length[j] = qlist[i]->edges[j]->length;

		vec3 d1, d2;
		d1 = vec3(qlist[i]->verts[0]->x, qlist[i]->verts[0]->y, qlist[i]->verts[0]->z);
		d2 = vec3(qlist[i]->verts[2]->x, qlist[i]->verts[2]->y, qlist[i]->verts[2]->z);
		double dia_length = length(d1 - d2);

		double temp_s1 = (edge_length[0] + edge_length[1] + dia_length) / 2.0;
		double temp_s2 = (edge_length[2] + edge_length[3] + dia_length) / 2.0;
		qlist[i]->area = sqrt(temp_s1*(temp_s1 - edge_length[0])*(temp_s1 - edge_length[1])*(temp_s1 - dia_length)) +
			sqrt(temp_s2*(temp_s2 - edge_length[2])*(temp_s2 - edge_length[3])*(temp_s2 - dia_length));

		area += qlist[i]->area;
		temp_q = qlist[i];
		v1 = vec3(vlist[qlist[i]->verts[0]->index]->x, vlist[qlist[i]->verts[0]->index]->y, vlist[qlist[i]->verts[0]->index]->z);
		v2 = vec3(vlist[qlist[i]->verts[1]->index]->x, vlist[qlist[i]->verts[1]->index]->y, vlist[qlist[i]->verts[1]->index]->z);
		v0 = vec3(vlist[qlist[i]->verts[2]->index]->x, vlist[qlist[i]->verts[2]->index]->y, vlist[qlist[i]->verts[2]->index]->z);
		qlist[i]->normal = cross(v0 - v1, v2 - v1);
		normalize(qlist[i]->normal);
	}

	double signedvolume = 0.0;
	vec3 test = center;
	for (i = 0; i < nquads; i++) {
		vec3 cent(vlist[qlist[i]->verts[0]->index]->x, vlist[qlist[i]->verts[0]->index]->y, vlist[qlist[i]->verts[0]->index]->z);
		signedvolume += dot(test - cent, qlist[i]->normal)*qlist[i]->area;
	}
	signedvolume /= area;
	if (signedvolume < 0)
		orientation = 0;
	else {
		orientation = 1;
		for (i = 0; i < nquads; i++)
			qlist[i]->normal *= -1.0;
	}
}

void sort(unsigned int *A, unsigned int *B, unsigned int *C, unsigned int sid, unsigned int eid) {
	unsigned int i;
	unsigned int *tempA, *tempB, *tempC;
	unsigned int current1, current2, current0;

	if (sid >= eid)
		return;
	sort(A, B, C, sid, (sid + eid) / 2);
	sort(A, B, C, (sid + eid) / 2 + 1, eid);
	tempA = (unsigned int *)malloc(sizeof(unsigned int)*(eid - sid + 1));
	tempB = (unsigned int *)malloc(sizeof(unsigned int)*(eid - sid + 1));
	tempC = (unsigned int *)malloc(sizeof(unsigned int)*(eid - sid + 1));
	for (i = 0; i < eid - sid + 1; i++) {
		tempA[i] = A[i + sid];
		tempB[i] = B[i + sid];
		tempC[i] = C[i + sid];
	}
	current1 = sid;
	current2 = (sid + eid) / 2 + 1;
	current0 = sid;
	while ((current1 <= (sid + eid) / 2) && (current2 <= eid)) {
		if (tempA[current1 - sid] < tempA[current2 - sid]) {
			A[current0] = tempA[current1 - sid];
			B[current0] = tempB[current1 - sid];
			C[current0] = tempC[current1 - sid];
			current1++;
		}
		else if (tempA[current1 - sid] > tempA[current2 - sid]) {
			A[current0] = tempA[current2 - sid];
			B[current0] = tempB[current2 - sid];
			C[current0] = tempC[current2 - sid];
			current2++;
		}
		else {
			if (tempB[current1 - sid] < tempB[current2 - sid]) {
				A[current0] = tempA[current1 - sid];
				B[current0] = tempB[current1 - sid];
				C[current0] = tempC[current1 - sid];
				current1++;
			}
			else {
				A[current0] = tempA[current2 - sid];
				B[current0] = tempB[current2 - sid];
				C[current0] = tempC[current2 - sid];
				current2++;
			}
		}
		current0++;
	}
	if (current1 <= (sid + eid) / 2) {
		for (i = current1; i <= (sid + eid) / 2; i++) {
			A[current0] = tempA[i - sid];
			B[current0] = tempB[i - sid];
			C[current0] = tempC[i - sid];
			current0++;
		}
	}
	if (current2 <= eid) {
		for (i = current2; i <= eid; i++) {
			A[current0] = tempA[i - sid];
			B[current0] = tempB[i - sid];
			C[current0] = tempC[i - sid];
			current0++;
		}
	}

	free(tempA);
	free(tempB);
	free(tempC);
}

Polyhedron::Polyhedron()
{
	nverts = nedges = nquads = 0;
	max_verts = max_quads = 50;
	vlist = new Vertex *[max_verts];
	qlist = new Quad *[max_quads];
}

void Polyhedron::average_normals()
{
	int i, j;

	for (i = 0; i < nverts; i++) {
		vlist[i]->normal = vec3(0.0);
		for (j = 0; j < vlist[i]->nquads; j++)
			vlist[i]->normal += vlist[i]->quads[j]->normal;
		normalize(vlist[i]->normal);
	}
}
