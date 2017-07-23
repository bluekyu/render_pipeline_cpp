#include "rppanda/interval/config_rppanda_interval.hpp"

#include "dconfig.h"

#include "render_pipeline/rppanda/interval/actor_interval.hpp"
#include "render_pipeline/rppanda/interval/lerp_interval.hpp"
#include "render_pipeline/rppanda/interval/meta_interval.hpp"
#include "render_pipeline/rppanda/interval/sound_interval.hpp"

Configure(config_rppanda_interval);
NotifyCategoryDef(rppanda_interval, "");

ConfigureFn(config_rppanda_interval)
{
    init_librppanda_interval();
}

void init_librppanda_interval()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    initialized = true;

    rppanda::ActorInterval::init_type();

    rppanda::LerpNodePathInterval::init_type();
    rppanda::LerpPosInterval::init_type();
    rppanda::LerpHprInterval::init_type();
    rppanda::LerpQuatInterval::init_type();
    rppanda::LerpScaleInterval::init_type();
    rppanda::LerpShearInterval::init_type();
    rppanda::LerpPosHprInterval::init_type();

    rppanda::MetaInterval::init_type();
    rppanda::Sequence::init_type();
    rppanda::Parallel::init_type();
    rppanda::ParallelEndTogether::init_type();
    rppanda::Track::init_type();

    rppanda::SoundInterval::init_type();
}
