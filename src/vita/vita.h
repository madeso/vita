#pragma once

#include <memory>

#include "imgui.h"

#include "vita/dependency_sdl.h"

struct App
{
	virtual void on_frame() = 0;
	virtual void on_gui() = 0;
	virtual void on_render(int x, int y, float dt) = 0;

	virtual ~App() = default;
};

using MakeAppFunction = std::unique_ptr<App> (*)();
int run_main(MakeAppFunction make_app);

#define IMPLEMENT_MAIN(APP) \
	int main(int, char**) \
	{ \
		return run_main([]() -> std::unique_ptr<App> { return std::make_unique<APP>(); }); \
	}
