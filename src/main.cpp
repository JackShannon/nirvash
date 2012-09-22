#define GLM_SWIZZLE
#include <GL/glcorearb.h>
#include <GL/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "state.h"
#include "window.h"
#include "resourceloader.h"
#include "fx/postprocess.h"
#include "material/material.h"
#include "model/model.h"
#include "utils/thread.h"
#include "utils/logger.h"

const int num_buffers = 1;

// Calculate FXAA alpha for a given color
float calc_alpha(vec3 in)
{
	using namespace glm;
	return dot(in, vec3(0.299, 0.587, 0.114));
}


void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window *w;
	Nepgear::Logger log(NULL);

	Nepgear::ResourceLoader<Nepgear::Mesh> ml;
	//ml.queue.push_back("Nepgear/nepgear.dae");
	ml.queue.push_back("monkey.dae");
	ml.Process();
	
	// wait for the window to be created.
	while(ng->windows.empty());

	w = ng->windows[0];

	// Bind the OpenGL context to this thread
	w->MakeCurrent();
	w->VSync(true);
	if (!w->Prepare(3, 2))
	{
		log.warn(
			"OpenGL 3.2 is not supported. Please check your drivers"
			"or hardware for support."
		);
		w->ClearCurrent();
		return;
	}
	
	// HACK: NVIDIA 295.20 drivers specifically doesn't wait properly.
	if (strcmp((const char*)glGetString(GL_VERSION), "3.2.0 NVIDIA 295.20")==0)
		ng->configuration["enable_wait_hack"] = true;
	ng->start = true;
	
	std::vector<Nepgear::Model*> render_queue;

	Nepgear::Material mat;
	mat.load("test.glsl");
	mat.bind();
	
	glm::mat4 view(1.0);
	glm::mat4 projection = glm::perspective(70.0f, 1.6f, 1.0f, 1000.0f);

	view = glm::translate(view, vec3(0.0f, -25.0f, -70.0f));
	view = glm::rotate(view, -80.f, vec3(1.0, 0.0, 0.0));
	
	mat.set_uniform_vec3(
		"LightDirection",
		glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f))
	);
	mat.set_uniform_mat4("View", view);
	mat.set_uniform_mat4("Projection", projection);

	Nepgear::PostProcessEffect fxaa;
	fxaa.init(w->width, w->height);
	fxaa.load("fxaa.glsl");

	glm::vec4 clear = glm::vec4(0.1, 0.4, 0.8, 1.0);
	clear.a = calc_alpha(clear.rgb());

	glClearColor(clear.r, clear.g, clear.b, clear.a);

	glEnable(GL_DEPTH_TEST);

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
				m->SetMaterial(&mat);
				m->UploadMesh();

				render_queue.push_back(m);
			}
			if (glGetError())
			{
				log.warn("GL error while loading resources!\n");
				break;
			}
		}

		fxaa.bind();

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// TODO: working per-mesh materials!
		mat.bind();

		auto it = render_queue.begin();
		for ( ; it != render_queue.end(); ++it)
		{
			(*it)->Update(delta);
			for (int i = 0; i<num_buffers; i++)
				(*it)->Draw(i);
		}

		fxaa.unbind();

		fxaa.draw();

		w->SwapBuffers();

		// HACK: glfwWaitEvents() workaround.
		if (ng->configuration["enable_wait_hack"])
		{
			glfwPollEvents();
			// This only needs to happen for the first update!
			ng->configuration["enable_wait_hack"] = false;
		}
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

	ng->windows.push_back(&w);

	// wait for configuration to be worked out in the GL thread
	while (!ng->start);

	while (ng->running)
	{
		/* HACK: glfwWaitEvents misbehaves for me using nvidia's 295.20 drivers
		 * and a GT 330. Workaround: force poll on first update in other thread
		 */
		if (!ng->configuration["enable_wait_hack"])
			glfwWaitEvents();
		else
			tthread::this_thread::sleep_for(tthread::chrono::milliseconds(16));

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
