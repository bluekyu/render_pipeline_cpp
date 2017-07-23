#include "rppanda/showbase/config_rppanda_showbase.hpp"

#include "dconfig.h"

#include "render_pipeline/rppanda/showbase/direct_object.hpp"
#include "render_pipeline/rppanda/showbase/loader.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/showbase/sfx_player.hpp"

Configure(config_rppanda_showbase);
NotifyCategoryDef(rppanda_showbase, "");

ConfigureFn(config_rppanda_showbase)
{
    init_librppanda_showbase();
}

void init_librppanda_showbase()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    initialized = true;

    rppanda::DirectObject::init_type();
    rppanda::Loader::init_type();
    rppanda::ShowBase::init_type();
    rppanda::SfxPlayer::init_type();
}
