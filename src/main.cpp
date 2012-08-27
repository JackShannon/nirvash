#include <GL/gl3w.h>
#include <GL/glfw3.h>

#include "mesh.h"
#include "thread.h"
#include "state.h"
#include "window.h"
#include "resourceloader.h"
#include "entity.h"

namespace Nepgear
{
	class Model : public Entity
	{
	public:
		Model() : m_mesh(NULL) {}

		void SetMesh(Mesh *m)
		{
			m_mesh = m;
		}

		void UploadMesh()
		{
			if (m_mesh)
				UploadMeshResursive(m_mesh);
			else
				printf("bad juju\n");
		}
		
		void Update()
		{
			
		}

		void Draw()
		{
			if (!m_mesh) return;
			DrawRecursive(m_mesh);
		}

	protected:
		void UploadMeshResursive(Mesh *child)
		{
			if (!child->triangles.empty())
			{
				glGenVertexArrays(1, &child->vao);
				glBindVertexArray(child->vao);
	
				glGenBuffers(1, &child->buffer);
				glBindBuffer(GL_ARRAY_BUFFER, child->buffer);
				glBufferData(
					GL_ARRAY_BUFFER, child->triangles.size() * sizeof(Vertex),
					&child->triangles[0].position.x, GL_STATIC_DRAW
				);

				m_render_stack.push_back(child);
			}
			
			for (size_t i = 0; i < child->children.size(); i++)
			{
					UploadMeshResursive(&child->children[i]);
			}
		}

		void DrawRecursive(Mesh *child)
		{
			//glBindProgram(m_materials[child->material_id]);
			glBindVertexArray(child->vao);
			glDrawArrays(GL_TRIANGLES, 0, child->triangles.size() / 3);

			for (size_t i = 0; i < child->children.size(); i++)
			{
					DrawRecursive(&child->children[i]);
			}
		}

		Mesh *m_mesh;
		std::vector<Mesh*> m_render_stack;
	};
}

void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window *w;

	Nepgear::ResourceLoader<Nepgear::Mesh> ml;
	ml.queue.push_back("Nepgear/nepgear.dae");
	ml.Process();

	// wait for the window to be created.
	while(ng->windows.empty());

	w = ng->windows[0];

	// Bind the OpenGL context to this thread
	w->MakeCurrent();
	w->VSync(true);

	gl3wInit();
	if (!gl3wIsSupported(3, 2))
		printf("oh, shit\n");
	
	std::vector<Nepgear::Model*> render_queue;

	glClearColor(1.0, 0.0, 0.0, 1.0);
	while(ng->running)
	{
		if (ml.done)
		{
			// disable flag so we don't upload again.
			ml.done = false;
			
			auto it = ml.loaded.begin();
			for ( ; it != ml.loaded.end(); ++it)
			{
				Nepgear::Model *m = new Nepgear::Model();
				m->SetMesh(*it);
				m->UploadMesh();

				render_queue.push_back(m);
			}
			
			glClearColor(0.0, 1.0, 0.0, 1.0);
		}
		glClear(GL_COLOR_BUFFER_BIT);

		printf("queue size: %ld\n", render_queue.size());
		
		auto it = render_queue.begin();
		for ( ; it != render_queue.end(); ++it)
		{
			(*it)->Draw();
		}
		
		w->SwapBuffers();

		//glfwPollEvents(); // HACK: glfwWaitEvents() fucks up this loop? What?
	}

	auto it = render_queue.begin();
	for ( ; it != render_queue.end(); ++it)
	{
		delete *it;
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
		glfwWaitEvents(); // NOTE: broken

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
