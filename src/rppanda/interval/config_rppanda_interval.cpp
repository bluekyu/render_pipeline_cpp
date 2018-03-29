/**
 * Render Pipeline C++
 *
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "rppanda/interval/config_rppanda_interval.hpp"

#include <dconfig.h>

#include "render_pipeline/rppanda/actor/actor.hpp"
#include "render_pipeline/rppanda/interval/actor_interval.hpp"
#include "render_pipeline/rppanda/interval/lerp_interval.hpp"
#include "render_pipeline/rppanda/interval/meta_interval.hpp"
#include "render_pipeline/rppanda/interval/sound_interval.hpp"

Configure(config_rppanda_interval);
NotifyCategoryDef(rppanda_interval, "");

ConfigureFn(config_rppanda_interval)
{
    static bool initialized = false;
    if (initialized)
        return;
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
