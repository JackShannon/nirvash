#ifndef _NEPGEAR_MESH_H_
#define _NEPGEAR_MESH_H_

#include <vector>
#include <glm/glm.hpp>

namespace Nepgear
{

struct Bone
{
	Bone() : previous(NULL), next(NULL) {}
	int id;
	glm::vec3 position;
	glm::vec4 orientation;
	Bone *previous, *next;
};

/* position, normal, texcoords, associated bone and weight */
/*
struct Vertex
{
	glm::vec3 position, normal;
	glm::vec2 tex_coords[4];
	unsigned bone;
	float weight;
};
*/
/* stores 3 vertex indices and a face normal */
struct Face
{
	int v1, v2, v3;
	glm::vec3 normal;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
	glm::vec4 color;
};

struct Mesh
{
	Mesh() : vao(0), buffer(0) {}

	int material_id;

	std::vector<Vertex> points;
	std::vector<Vertex> lines;
	std::vector<Vertex> triangles;

	glm::mat4 xform;

	Mesh *parent;
	std::vector<Mesh> children;

	// GL stuff
	unsigned vao;
	unsigned buffer;
};

}

#endif
