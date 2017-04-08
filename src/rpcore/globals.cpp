#include <render_pipeline/rpcore/globals.h>

#include <clockObject.h>

#include <render_pipeline/rppanda/showbase/showbase.h>

namespace rpcore {

rppanda::ShowBase* Globals::base = nullptr;
NodePath Globals::render;
ClockObject* Globals::clock = nullptr;
LVecBase2i Globals::resolution;
LVecBase2i Globals::native_resolution;
PT(TextFont) Globals::font = nullptr;

void Globals::load(rppanda::ShowBase* showbase)
{
	Globals::base = showbase;
	Globals::render = showbase->get_render();
	Globals::clock = ClockObject::get_global_clock();
	Globals::resolution = LVecBase2i(0, 0);
}

}	// namespace rpcore
