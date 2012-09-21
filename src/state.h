#ifndef _NEPGEAR_STATE_H_
#define _NEPGEAR_STATE_H_

#include "utils/thread.h"
#include "window.h"
#include "inputstate.h"
#include <map>
#include <vector>

// Nepgear/state.h
namespace Nepgear
{
	struct State
	{
		State() : start(false), running(true) {}

		Mutex lock;

		std::map<std::string, Thread*> threads;
		std::vector<Window*> windows;

		std::map<std::string, bool> configuration;

		InputState input;

		bool start;
		bool running;
	};
}

#endif
