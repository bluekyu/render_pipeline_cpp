#include "rppanda/actor/config_rppanda_actor.hpp"

#include "dconfig.h"

#include "render_pipeline/rppanda/actor/actor.hpp"

Configure(config_rppanda_actor);
NotifyCategoryDef(rppanda_actor, "");

ConfigureFn(config_rppanda_actor)
{
    init_librppanda_actor();
}

void init_librppanda_actor()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    initialized = true;

    rppanda::Actor::init_type();
}
