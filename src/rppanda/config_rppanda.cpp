#include "rppanda/config_rppanda.hpp"

#include <render_pipeline/rppanda/interval/meta_interval.hpp>
#include <render_pipeline/rppanda/interval/lerp_interval.hpp>

#include "dconfig.h"

Configure(config_rppanda);
NotifyCategoryDef(rppanda, "");

ConfigureFn(config_rppanda)
{
	init_librppanda();
}

void init_librppanda()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }

    initialized = true;

	rppanda::MetaInterval::init_type();
	rppanda::Sequence::init_type();
	rppanda::Parallel::init_type();
	rppanda::ParallelEndTogether::init_type();
	rppanda::Track::init_type();

	rppanda::LerpNodePathInterval::init_type();
	rppanda::LerpPosInterval::init_type();
	rppanda::LerpHprInterval::init_type();
	rppanda::LerpQuatInterval::init_type();
	rppanda::LerpScaleInterval::init_type();
	rppanda::LerpShearInterval::init_type();
	rppanda::LerpPosHprInterval::init_type();
}
