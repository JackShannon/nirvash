#ifndef _NEPGEAR_WINDOW_H_
#define _NEPGEAR_WINDOW_H_

// Nepgear/window.h
namespace Nepgear
{
	typedef void* WindowHandle;
	
	struct WindowFlags
	{
		int width, height;
		int gl_major, gl_minor;
		bool strict;
		enum {
			FullScreen = 0,
			Windowed,
			Invalid
		} mode;
		WindowHandle *homie;
	};

	class Window
	{
	public:
		void Create(WindowFlags flags);
		void Destroy();
		void MakeCurrent();

		//void Resize(glm::vec2);

		void VSync(bool);
		void SwapBuffers();
		
		WindowHandle GetHandle();
		
	protected:
		WindowHandle m_handle;
	};
}

#endif
