#include "thread.h"
#include "state.h"
#include "window.h"
#include <GL/glfw3.h>

void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window *w;

	// wait for the window to be created.
	while(ng->windows.empty());

	w = ng->windows[0];

	// Bind the OpenGL context to this thread
	w->MakeCurrent();
	w->VSync(true);
	
	glClearColor(1.0, 0.0, 0.0, 1.0);
	while(ng->running)
	{
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
