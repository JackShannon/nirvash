#ifndef _NEPGEAR_MODEL_H_
#define _NEPGEAR_MODEL_H_

#include "entity.h"
#include "mesh.h"
#include "material.h"

namespace Nepgear
{
	class Model : public Entity
	{
	public:
		Model() : m_mesh(NULL) {}

		void SetMesh(Mesh *m);
		void SetMaterial(Material *m);
		void UploadMesh();
		void Update(double delta);
		void Draw();

		Material *m_material;
	protected:
		void UploadMeshResursive(Mesh *child);
		void DrawRecursive(Mesh *child);

		Mesh *m_mesh;
		std::vector<Mesh*> m_render_stack;
	};
}

#endif
