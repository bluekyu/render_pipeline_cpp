#include "render_pipeline/rppanda/actor/actor.h"

#include <loader.h>

namespace rppanda {

Actor::Actor(void)
{
	loader_ = Loader::get_global_ptr();
}

}
