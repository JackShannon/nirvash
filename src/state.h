#ifndef _NEPGEAR_STATE_H_
#define _NEPGEAR_STATE_H_

#include "thread.h"
#include <map>

// Nepgear/state.h
namespace Nepgear
{
	struct State
	{
		State() : running(true) {}

		Mutex lock;
		
		std::map<std::string, Thread*> threads;

		int wait()
		{
			auto it = threads.begin();
			for ( ; it != threads.end(); ++it)
			{
				if (!it->second)
					continue;
				it->second->join();
				delete it->second;
			}

			return 0;
		}
		
		bool running;
	};
}

#endif
