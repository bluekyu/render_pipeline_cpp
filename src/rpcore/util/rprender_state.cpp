/**
 * Render Pipeline C++
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence.
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

#include "render_pipeline/rpcore/util/rprender_state.hpp"

#include <nodePath.h>
#include <materialAttrib.h>
#include <textureAttrib.h>

#include <fmt/ostream.h>

#include "render_pipeline/rpcore/rpobject.hpp"

namespace rpcore {

RPRenderState::RPRenderState(const NodePath& nodepath)
{
    state_ = nodepath.node()->get_state();
}

RPRenderState::~RPRenderState() = default;

bool RPRenderState::has_material() const
{
    return state_->has_attrib(MaterialAttrib::get_class_type());
}

RPMaterial RPRenderState::get_material() const
{
    if (!state_->has_attrib(MaterialAttrib::get_class_type()))
        return RPMaterial(nullptr);

    return RPMaterial(DCAST(MaterialAttrib, state_->get_attrib(MaterialAttrib::get_class_type()))->get_material());
}

RPRenderState& RPRenderState::set_material(Material* material)
{
    state_ = state_->set_attrib(MaterialAttrib::make(material));
    return *this;
}

bool RPRenderState::has_texture() const
{
    return state_->has_attrib(TextureAttrib::get_class_type());
}

Texture* RPRenderState::get_texture() const
{
    if (!state_->has_attrib(TextureAttrib::get_class_type()))
        return nullptr;

    return DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()))->get_texture();
}

Texture* RPRenderState::get_texture(TextureStage* stage) const
{
    if (!state_->has_attrib(TextureAttrib::get_class_type()))
        return nullptr;

    return DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()))->get_on_texture(stage);
}

Texture* RPRenderState::get_basecolor_texture() const
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        if (tex_attrib->get_num_on_stages() > static_cast<int>(TextureStageIndex::basecolor))
            return tex_attrib->get_on_texture(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::basecolor)));
    }

    return nullptr;
}

Texture* RPRenderState::get_normal_texture() const
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        if (tex_attrib->get_num_on_stages() > static_cast<int>(TextureStageIndex::normal))
            return tex_attrib->get_on_texture(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::normal)));
    }

    return nullptr;
}

Texture* RPRenderState::get_specular_texture() const
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        if (tex_attrib->get_num_on_stages() > static_cast<int>(TextureStageIndex::specular))
            return tex_attrib->get_on_texture(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::specular)));
    }

    return nullptr;
}

Texture* RPRenderState::get_roughness_texture() const
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        if (tex_attrib->get_num_on_stages() > static_cast<int>(TextureStageIndex::roughness))
            return tex_attrib->get_on_texture(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::roughness)));
    }

    return nullptr;
}

RPRenderState& RPRenderState::set_texture(Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(tex_attrib->filter_to_max(1)->get_on_stage(0), texture, priority);
        state_ = state_->set_attrib(new_attrib, state_->get_override(TextureAttrib::get_class_type()));
    }
    else
    {
        CPT(RenderAttrib) new_attrib = TextureAttrib::make(texture);
        state_ = state_->set_attrib(new_attrib);
    }

    return *this;
}

RPRenderState& RPRenderState::set_texture(TextureStage* stage, Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        state_ = state_->set_attrib(new_attrib, state_->get_override(TextureAttrib::get_class_type()));
    }
    else
    {
        CPT(TextureAttrib) new_attrib = DCAST(TextureAttrib, TextureAttrib::make());
        state_ = state_->set_attrib(new_attrib->add_on_stage(stage, texture, priority));
    }

    return *this;
}

RPRenderState& RPRenderState::set_basecolor_texture(Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));
        auto stage = tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::basecolor));

        CPT(RenderAttrib) new_attrib = tex_attrib->add_on_stage(stage, texture, priority);
        state_ = state_->set_attrib(new_attrib, state_->get_override(TextureAttrib::get_class_type()));
    }
    else
    {
        CPT(TextureAttrib) new_attrib = DCAST(TextureAttrib, TextureAttrib::make());
        PT(TextureStage) stage = new TextureStage(texture->get_name() + "-basecolor");
        stage->set_sort(0);
        state_ = state_->set_attrib(new_attrib->add_on_stage(stage, texture, priority));
    }

    return *this;
}

RPRenderState& RPRenderState::set_normal_texture(Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        const auto num_on_stages = tex_attrib->get_num_on_stages();
        if (num_on_stages > static_cast<int>(TextureStageIndex::normal))
        {
            stage = tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::normal));
        }
        else if (num_on_stages > (static_cast<int>(TextureStageIndex::normal)-1))
        {
            stage = new TextureStage(texture->get_name() + "-normal");
            stage->set_sort(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::normal)-1)->get_sort() + 1);
        }
        else
        {
            RPObject::global_error("RPRenderState", "The state does NOT have 'basecolor' texture.");
        }

        if (stage)
        {
            state_ = state_->set_attrib(tex_attrib->add_on_stage(stage, texture, priority),
                state_->get_override(TextureAttrib::get_class_type()));
        }
    }
    else
    {
        RPObject::global_error("RPRenderState", "The state does NOT have any texture.");
    }

    return *this;
}

RPRenderState& RPRenderState::set_specular_texture(Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        const auto num_on_stages = tex_attrib->get_num_on_stages();
        if (num_on_stages > static_cast<int>(TextureStageIndex::specular))
        {
            stage = tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::specular));
        }
        else if (num_on_stages > (static_cast<int>(TextureStageIndex::specular)-1))
        {
            stage = new TextureStage(texture->get_name() + "-specular");
            stage->set_sort(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::specular)-1)->get_sort() + 1);
        }
        else
        {
            RPObject::global_error("RPRenderState", "The state does NOT have 'basecolor' or 'normal' texture.");
        }

        if (stage)
        {
            state_ = state_->set_attrib(tex_attrib->add_on_stage(stage, texture, priority),
                state_->get_override(TextureAttrib::get_class_type()));
        }
    }
    else
    {
        RPObject::global_error("RPRenderState", "The state does NOT have any texture.");
    }

    return *this;
}

RPRenderState& RPRenderState::set_roughness_texture(Texture* texture, int priority)
{
    if (state_->has_attrib(TextureAttrib::get_class_type()))
    {
        const TextureAttrib* tex_attrib = DCAST(TextureAttrib, state_->get_attrib(TextureAttrib::get_class_type()));

        PT(TextureStage) stage;
        const auto num_on_stages = tex_attrib->get_num_on_stages();
        if (num_on_stages > static_cast<int>(TextureStageIndex::roughness))
        {
            stage = tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::roughness));
        }
        else if (num_on_stages > (static_cast<int>(TextureStageIndex::roughness)-1))
        {
            stage = new TextureStage(texture->get_name() + "-roughness");
            stage->set_sort(tex_attrib->get_on_stage(static_cast<int>(TextureStageIndex::roughness)-1)->get_sort() + 1);
        }
        else
        {
            RPObject::global_error("RPRenderState", "The state does NOT have 'basecolor', 'normal' or 'specular' texture.");
        }

        if (stage)
        {
            state_ = state_->set_attrib(tex_attrib->add_on_stage(stage, texture, priority),
                state_->get_override(TextureAttrib::get_class_type()));
        }
    }
    else
    {
        RPObject::global_error("RPRenderState", "The state does NOT have any texture.");
    }

    return *this;
}

}
