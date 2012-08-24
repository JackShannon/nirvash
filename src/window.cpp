#include "window.h"
#include <GL/glfw3.h>

// Nepgear/window.cpp
namespace Nepgear
{
	void Window::MakeCurrent()
	{
		glfwMakeContextCurrent((GLFWwindow)m_handle);
	}
	
	WindowHandle Window::GetHandle()
	{
		return m_handle;
	}

	void Window::Create(WindowFlags flags)
	{
		int mode = (flags.mode == WindowFlags::FullScreen) ? GLFW_FULLSCREEN : GLFW_WINDOWED;
		glfwInit();

		if (flags.gl_major >= 3)
		{
			if (flags.strict)
			{
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
			} else {
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
			}
			glfwWindowHint(GLFW_OPENGL_VERSION_MAJOR, flags.gl_major);
			glfwWindowHint(GLFW_OPENGL_VERSION_MINOR, flags.gl_minor);
		}
		m_handle = (WindowHandle)glfwCreateWindow(
			flags.width, flags.height,
			mode, "", (GLFWwindow)flags.homie
		);
		MakeCurrent();
	}

	void Window::Destroy()
	{
		glfwDestroyWindow((GLFWwindow)m_handle);
	}

	void Window::VSync(bool enabled)
	{
		glfwSwapInterval((int)enabled);
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers((GLFWwindow)m_handle);
	}
}
