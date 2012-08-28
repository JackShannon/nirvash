#include <GL/gl3w.h>
#include <GL/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "thread.h"
#include "state.h"
#include "window.h"
#include "resourceloader.h"
#include "material.h"
#include "model.h"
#include "logger.h"

void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window *w;
	Nepgear::Logger log(NULL);

	Nepgear::ResourceLoader<Nepgear::Mesh> ml;
	ml.queue.push_back("monkey.dae");
	ml.Process();
	
	// wait for the window to be created.
	while(ng->windows.empty());

	w = ng->windows[0];

	// Bind the OpenGL context to this thread
	w->MakeCurrent();
	w->VSync(true);

	gl3wInit();
	if (!gl3wIsSupported(3, 2))
	{
		log.warn(
			"OpenGL 3.2 is not supported. Please check your drivers"
			"or hardware for support."
		);
		w->ClearCurrent();
		return;
	}
	
	std::vector<Nepgear::Model*> render_queue;

	Nepgear::Material mat;
	mat.load("test.glsl");
	mat.bind();
	
	glm::mat4 mvp(1.0);
	glm::mat4 projection = glm::perspective(70.0f, 1.6f, 1.0f, 1000.0f);

	mvp = glm::translate(mvp, vec3(0.0f, 0.0f, -10.0f));
	mvp = projection * mvp;
	
	mat.set_uniform_vec3("LightDirection", glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f)));
	mat.set_uniform_mat4("ModelViewProjection", mvp);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	double now = glfwGetTime();
	double then = now;
	double delta = 0.0;
	while(ng->running)
	{
		now = glfwGetTime();
		delta = now - then;
		then = now;

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
			if (glGetError())
			{
				log.warn("GL error while loading resources!\n");
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		auto it = render_queue.begin();
		for ( ; it != render_queue.end(); ++it)
		{
			(*it)->Update(delta);
			(*it)->Draw();
		}
		
		w->SwapBuffers();

		glfwPollEvents(); // HACK: glfwWaitEvents() fucks up this loop? What?
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
