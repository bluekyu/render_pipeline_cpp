/**
 * Render Pipeline C++
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
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

#include "render_pipeline/rpcore/util/basic_effects.hpp"

namespace rpcore {

const Effect::SourceType& get_default_effect_source()
{
    static const Effect::SourceType effect_source = {
        "/$$rp/effects/default.yaml",
        {}
    };

    return effect_source;
}

const Effect::SourceType& get_transparent_effect_source()
{
    static const Effect::SourceType effect_source = {
        "/$$rp/effects/default.yaml",
        {
            {Effect::pass_option_prefix + std::string("forward"), true},
            {Effect::pass_option_prefix + std::string("gbuffer"), false}
        }
    };

    return effect_source;
}

}
