/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#include "render_pipeline/rppanda/actor/actor.hpp"

#include <loader.h>

namespace rppanda {

Actor::Actor(void)
{
	loader_ = Loader::get_global_ptr();
}

}
