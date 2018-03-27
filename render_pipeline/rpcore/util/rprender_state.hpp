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

#pragma once

#include <render_pipeline/rpcore/config.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

class Texture;
class NodePath;
class RenderState;
class TextureStage;

namespace rpcore {

/**
 * Adapater of RenderState.
 */
class RENDER_PIPELINE_DECL RPRenderState
{
public:
    enum class TextureStageIndex : int
    {
        basecolor = 0,
        normal,
        specular,
        roughness,
    };

public:
    RPRenderState(const NodePath& nodepath);
    RPRenderState(CPT(RenderState) state);

    ~RPRenderState();

    explicit operator bool() const;
    bool operator!() const;

    const RenderState* get_state() const;

    bool has_material() const;
    RPMaterial get_material() const;
    RPRenderState& set_material(const RPMaterial& material);
    RPRenderState& set_material(Material* material);

    bool has_texture() const;
    Texture* get_texture() const;
    Texture* get_texture(TextureStage* stage) const;

    /**
     * Get the texture on specific type.
     * @return  Texture pointer or nullptr if empty or failed.
     */
    Texture* get_basecolor_texture() const;
    Texture* get_normal_texture() const;
    Texture* get_specular_texture() const;
    Texture* get_roughness_texture() const;

    /** Set the texture on the first TextureStage or default stage if texture does not exist. */
    RPRenderState& set_texture(Texture* texture, int priority = 0);

    /** Set the texture on the given TextureStage. */
    RPRenderState& set_texture(TextureStage* stage, Texture* texture, int priority = 0);

    /** Set the texture on specific type. */
    RPRenderState& set_basecolor_texture(Texture* texture, int priority = 0);
    RPRenderState& set_normal_texture(Texture* texture, int priority = 0);
    RPRenderState& set_specular_texture(Texture* texture, int priority = 0);
    RPRenderState& set_roughness_texture(Texture* texture, int priority = 0);

protected:
    CPT(RenderState) state_;
};

// ************************************************************************************************

inline RPRenderState::RPRenderState(CPT(RenderState) state) : state_(state)
{
}

inline RPRenderState::operator bool() const
{
    return state_ != nullptr;
}

inline bool RPRenderState::operator!() const
{
    return state_ == nullptr;
}

inline const RenderState* RPRenderState::get_state() const
{
    return state_;
}

inline RPRenderState& RPRenderState::set_material(const RPMaterial& material)
{
    return set_material(material.get_material());
}

// ************************************************************************************************

inline bool operator==(const RPRenderState& a, const RPRenderState& b)
{
    return a.get_state() == b.get_state();
}

inline bool operator!=(const RPRenderState& a, const RPRenderState& b)
{
    return a.get_state() != b.get_state();
}

inline bool operator==(const RPRenderState& m, std::nullptr_t)
{
    return m.get_state() == nullptr;
}

inline bool operator==(std::nullptr_t, const RPRenderState& m)
{
    return m.get_state() == nullptr;
}

inline bool operator!=(const RPRenderState& m, std::nullptr_t)
{
    return m.get_state() != nullptr;
}

inline bool operator!=(std::nullptr_t, const RPRenderState& m)
{
    return m.get_state() != nullptr;
}

}
