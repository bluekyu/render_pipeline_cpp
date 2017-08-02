/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
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

#include "rpcore/util/ies_profile_loader.hpp"

#include <shaderInput.h>

#include "render_pipeline/rpcore/render_pipeline.hpp"
#include "render_pipeline/rpcore/stage_manager.hpp"

namespace rpcore {

const std::vector<std::string> IESProfileLoader::PROFILES = {
    "IESNA:LM-63-1986",
    "IESNA:LM-63-1991",
    "IESNA91",
    "IESNA:LM-63-1995",
    "IESNA:LM-63-2002",
    "ERCO Leuchten GmbH  BY: ERCO/LUM650/8701",
    "ERCO Leuchten GmbH"
};

size_t IESProfileLoader::load(const std::string& filename)
{
    // TODO: implement
    return 0;
}

void IESProfileLoader::create_storage(void)
{
    storage_tex_ = Image::create_3d("IESDatasets", 512, 512, max_entries_, "R16");
    storage_tex_->set_minfilter(SamplerState::FT_linear);
    storage_tex_->set_magfilter(SamplerState::FT_linear);
    storage_tex_->set_wrap_u(SamplerState::WM_clamp);
    storage_tex_->set_wrap_v(SamplerState::WM_repeat);
    storage_tex_->set_wrap_w(SamplerState::WM_clamp);

    pipeline_.get_stage_mgr()->add_input(ShaderInput("IESDatasetTex", storage_tex_->get_texture()));
    pipeline_.get_stage_mgr()->get_defines()["MAX_IES_PROFILES"] = std::to_string(max_entries_);
}

}
