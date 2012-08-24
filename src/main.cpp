#include "thread.h"
#include "state.h"
#include "window.h"
#include <GL/glfw3.h>

Nepgear::Mutex m;

void start_video(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;
	Nepgear::Window w;
	Nepgear::WindowFlags f;
	{
		f.width = 960;
		f.height = 540;
		f.gl_major = 3;
		f.gl_minor = 3;
		f.strict = true;
		f.mode = Nepgear::WindowFlags::Windowed;
		f.homie = NULL;
	}
	w.Create(f);
	
	//tthread::this_thread::sleep_for(tthread::chrono::milliseconds(2000));

	while(ng->running)
	{
		glfwPollEvents();
		if (glfwGetKey(w.GetHandle(), GLFW_KEY_ESC) == GLFW_PRESS)
		{
			Nepgear::LockGuard<Nepgear::Mutex>::type hold(ng->lock);
			ng->running = false;
			continue;
		}
		w.SwapBuffers();
	}

	w.Destroy();
}

void start_audio(void *data)
{
	Nepgear::State *ng = (Nepgear::State*)data;

	// silence warning.
	while (ng->running) break;
}

void init_game(Nepgear::State *ng)
{
	// TODO
}

int main()
{
	Nepgear::State ng;

	ng.threads["video"] = new Nepgear::Thread(start_video, (void*)&ng);
	ng.threads["audio"] = new Nepgear::Thread(start_audio, (void*)&ng);

	init_game(&ng);

	return ng.wait();
}
