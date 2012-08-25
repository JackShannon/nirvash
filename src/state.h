#ifndef _NEPGEAR_STATE_H_
#define _NEPGEAR_STATE_H_

#include "thread.h"
#include "window.h"
#include "inputstate.h"
#include <map>
#include <vector>

// Nepgear/state.h
namespace Nepgear
{
	struct State
	{
		State() : running(true) {}

		Mutex lock;
		
		std::map<std::string, Thread*> threads;
		std::vector<Window*> windows;
		
		InputState input;
		
		bool running;
	};
}

#endif
