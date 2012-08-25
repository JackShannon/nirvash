#ifndef _NEPGEAR_MESH_H_
#define _NEPGEAR_MESH_H_

#include <GL3/gl3w.h>
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
struct Vertex
{
	glm::vec3 position, normal;
	glm::vec2 tex_coords[4];
	unsigned bone;
	float weight;
};

/* stores 3 vertex indices and a face normal */
struct Face
{
	int v1, v2, v3;
	glm::vec3 normal;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	std::vector<Bone> bones;
};

}

#endif
