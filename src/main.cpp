#include "thread.h"
#include "state.h"
#include "window.h"
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct ThreadStatus {
	ThreadStatus() : done(false), data(NULL) {}
	bool done;
	void *data;
};

namespace Nepgear
{
	namespace Convert
	{
		void ai_to_glm_mat4(glm::mat4 *out, aiMatrix4x4 in)
		{
			(*out)[0] = glm::vec4(in.a1, in.a2, in.a3, in.a4);
			(*out)[1] = glm::vec4(in.b1, in.b2, in.b3, in.b4);
			(*out)[2] = glm::vec4(in.c1, in.c2, in.c3, in.c4);
			(*out)[3] = glm::vec4(in.d1, in.d2, in.d3, in.d4);
		}

		void ai_to_glm_vec2(glm::vec2 *out, aiVector2t<float> in)
		{
			out->x = in.x;
			out->y = in.y;
		}

		void ai_to_glm_vec3(glm::vec3 *out, aiVector3t<float> in)
		{
			out->x = in.x;
			out->y = in.y;
			out->z = in.z;
		}

		void ai_to_glm_vec4(glm::vec4 *out, aiColor4t<float> in)
		{
			out->r = in.r;
			out->g = in.g;
			out->b = in.b;
			out->a = in.a;
		}
	}

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
		glm::vec4 color;
	};

	struct Mesh
	{
		Mesh() : depth(0) {}

		int material_id;

		std::vector<Vertex> points;
		std::vector<Vertex> lines;
		std::vector<Vertex> triangles;

		glm::mat4 xform;

		Mesh *parent;
		std::vector<Mesh> children;

		int depth;
	};
}

void recursive_load(Nepgear::Mesh *current, const struct aiScene *sc, const struct aiNode* nd)
{
	using namespace Nepgear::Convert;
	current->depth++;
//	printf("depth: %d\n", current->depth);
	ai_to_glm_mat4(&current->xform, nd->mTransformation);
	
	for (unsigned n = 0; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

		Nepgear::Mesh m;
		m.material_id = mesh->mMaterialIndex;

		printf("faces: %d\n", mesh->mNumFaces);

		for (unsigned t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];

			for(unsigned i = 0; i < face->mNumIndices; i++)
			{
				int idx = face->mIndices[i];
				Nepgear::Vertex v;

				ai_to_glm_vec3(&v.position, mesh->mVertices[idx]);

				if (mesh->mNormals != NULL)
					ai_to_glm_vec3(&v.normal, mesh->mNormals[idx]);

				if (mesh->HasTextureCoords(0))
				{
					glm::vec3 tmp;
					ai_to_glm_vec3(&tmp, mesh->mTextureCoords[0][idx]);
					v.tex_coords = glm::vec2(tmp.x, tmp.y);
				}

				if (mesh->mColors[0] != NULL)
					ai_to_glm_vec4(&v.color, mesh->mColors[0][idx]);

				std::vector<Nepgear::Vertex> *list;
				
				switch (face->mNumIndices)
				{
					case 1: list = &m.points; break;
					case 2: list = &m.lines; break;
					case 3: list = &m.triangles; break;
					default: continue; break;
				}
				list->push_back(v);
			}
		}
		m.parent = current;
		current->children.push_back(m);
	}
	
	for (unsigned n = 0; n < nd->mNumChildren; ++n)
	{
		if (current->children.size() <= n)
		{
			Nepgear::Mesh m;
			m.depth = current->depth;
			m.parent = current;
			current->children.push_back(m);
		}
		recursive_load(&current->children[n], sc, nd->mChildren[n]);
	}
}

void loader_thread(void *data)
{
	ThreadStatus *tdata = (ThreadStatus*)data;
	const aiScene *scene;

	Assimp::Importer imp;
	scene = imp.ReadFile(
		"Nepgear/nepgear.dae",
		aiProcess_Triangulate | aiProcess_GenSmoothNormals
	);

	if (!scene)
	{
		printf("it didn't work :(\n");
		tdata->data = NULL;
	}
	else
	{
		using namespace Nepgear::Convert;
		Nepgear::Mesh *mesh = new Nepgear::Mesh;
		ai_to_glm_mat4(&mesh->xform, scene->mRootNode->mTransformation);
		recursive_load(mesh, scene, scene->mRootNode);
		tdata->data = (void*)mesh;
	}

	tdata->done = true;
}

void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window *w;

	ThreadStatus ts;
	Nepgear::Thread t(loader_thread, (void*)&ts);

	// wait for the window to be created.
	while(ng->windows.empty());

	w = ng->windows[0];

	// Bind the OpenGL context to this thread
	w->MakeCurrent();
	w->VSync(true);

	glClearColor(1.0, 0.0, 0.0, 1.0);
	while(ng->running)
	{
		if (ts.done)
		{
			glClearColor(0.0, 1.0, 0.0, 1.0);
		}
		glClear(GL_COLOR_BUFFER_BIT);

		w->SwapBuffers();

		glfwPollEvents(); // HACK: glfwWaitEvents() fucks up this loop? What?
	}

	w->ClearCurrent();
}

void start_audio(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;

	// silence warning. terminate thread early
	while (ng->running) break;
}

void init_game(Nepgear::State *ng)
{
	Nepgear::Window w;
	Nepgear::WindowFlags f;
	{
		f.width = 960;
		f.height = 540;
		f.gl_major = 3;
		f.gl_minor = 2;
		f.strict = true;
		f.mode = Nepgear::WindowFlags::Windowed;
		f.homie = NULL;
	}
	w.Create(f);

	ng->lock.lock();
	ng->windows.push_back(&w);
	ng->lock.unlock();
	
	while (ng->running)
	{
		//glfwWaitEvents(); // NOTE: broken

		if (glfwGetKey(w.GetHandle(), GLFW_KEY_ESC) == GLFW_PRESS ||
			glfwGetWindowParam(w.GetHandle(), GLFW_CLOSE_REQUESTED))
		{
			Nepgear::LockGuard<Nepgear::Mutex>::type hold(ng->lock);
			ng->running = false;
		}
	}

	w.Destroy();
}

int main()
{
	Nepgear::State ng;

	glfwInit();

	ng.threads["video"] = new Nepgear::Thread(start_video, (void*)&ng);
	ng.threads["audio"] = new Nepgear::Thread(start_audio, (void*)&ng);

	init_game(&ng);

	auto it = ng.threads.begin();
	for ( ; it != ng.threads.end(); ++it)
	{
		if (!it->second)
			continue;
		it->second->join();
		delete it->second;
	}

	glfwTerminate();
	
	return 0;
}
