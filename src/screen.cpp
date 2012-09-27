#ifndef _NEPGEAR_SCREEN_H_
#define _NEPGEAR_SCREEN_H_

#include <string>
//#include "gameinput.h"
#include "state.h"

class GameInput;

namespace Nepgear
{
class Screen
{
public:
	Screen(std::string name, void *data);
	virtual ~Screen();

	std::string name;

	virtual void load(const State *ng);
	//virtual void quit();

	//virtual void focus();
	virtual void update(double dt);
	virtual void draw(int buffer);

	virtual void input(const GameInput *gi);

	void *data;
	struct {
		std::string name;
		void *data;
	} next;
};
}

#endif

//#include "gameinput.h"

#include <ostream>
#include <sstream>
#include <unistd.h>

namespace Nepgear
{

Screen::Screen(std::string _name, void *_data)
{
	name = _name;
	data = _data;
}

Screen::~Screen()
{
	// TODO
}

void Screen::load(const State *ng)
{
	UNUSED(ng);

	int err = 0;
	std::ostringstream file;
	file << "game/" << name << ".lua";

	if (access(file.str().c_str(), F_OK) == 0)
	{
		printf("File %s doesn't exist.\n", file.str().c_str());
		//return;
	}
	else
	{
		printf("always with the elses!");
	}

	luaL_loadfile(ng->lua, file.str().c_str());

	err = lua_pcall(ng->lua, 0, 0, 0);
	if (err)
	{
		switch (err)
		{
			case LUA_ERRRUN:
				printf("Lua: runtime error\n");
				break;
			case LUA_ERRMEM:
				printf("Lua: memory allocation error\n");
				break;
			default: break;
		}
		lua_pop(ng->lua, 1);
	}
}

void Screen::update(double dt)
{
	UNUSED(dt);
}

void Screen::draw(int buffer)
{
	UNUSED(buffer);
}

void Screen::input(const GameInput *gi)
{
	UNUSED(gi);
}

}
