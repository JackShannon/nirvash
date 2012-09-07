#define GLM_SWIZZLE
#include <GL/glcorearb.h>
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

namespace Nepgear
{
class PostProcessEffect
{
public:
	void init(int width, int height)
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0
		);

		// renderbuffer handle
		GLuint rbf;
		
		// generate renderbuffers
		glGenRenderbuffers(1, &rbf);
		glBindRenderbuffer(GL_RENDERBUFFER, rbf);
		glRenderbufferStorage(
			GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
			width, height
		);

		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0
		);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbf
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (vao != 0) return;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		GLfloat post_effect_vertexData[] = {
			 1.0f, 1.0f, 0.0f,       1.0f, 1.0f,
			-1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
			 1.0f,-1.0f, 0.0f,       1.0f, 0.0f,
			-1.0f,-1.0f, 0.0f,       0.0f, 0.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, post_effect_vertexData, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		GLuint post_effect_indexData[] = { 0,1,2, 2,1,3, };

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, post_effect_indexData, GL_STATIC_DRAW);
		//if(glGetError()) log.warn("problem! L#%d",__LINE__);
	}
	void load(std::string shader)
	{
		m_material.load(shader);
		m_material.bind();
		m_material.set_uniform_int("Texture", 0);
	}

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void draw()
	{
		m_material.bind();

		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glDisable(GL_DEPTH_TEST);
		glDisableVertexAttribArray(1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glEnableVertexAttribArray(1);
		glEnable(GL_DEPTH_TEST);
	}
	
	// more accurately: bind back buffer.
	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// we don't need different ones of these for each post effect;
	// they're just gonna be fullscreen quads,
	static unsigned vao, vbo, ibo;
protected:
	unsigned m_texture, m_fbo;
	Material m_material;
};
unsigned PostProcessEffect::vao = 0;
unsigned PostProcessEffect::vbo = 0;
unsigned PostProcessEffect::ibo = 0;
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
	clear.a = glm::dot(glm::vec3(clear.rgb()), glm::vec3(0.299, 0.587, 0.114));
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
			(*it)->Draw();
		}

		fxaa.unbind();
		fxaa.draw();

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

	ng->windows.push_back(&w);
	
	while (ng->running)
	{
		//glfwWaitEvents(); // NOTE: broken
		//tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));

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
