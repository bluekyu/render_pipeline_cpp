/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "window_interface.hpp"

namespace rppanda {
class Actor;
}

namespace rpplugins {

class LoadAnimationDialog;

class ActorWindow : public WindowInterface
{
public:
    ActorWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline);

    void draw_contents() final;

    void set_actor(rppanda::Actor* actor);

private:
    void actor_updated();

    void ui_load_animation();
    void ui_animation(const std::string& lod_name, const std::string& part_name);

    rppanda::Actor* actor_ = nullptr;
    rppanda::Actor::ActorInfoType actor_info_;

    std::unique_ptr<LoadAnimationDialog> load_animation_dialog_;

    std::vector<std::string> blend_type_cache_list_;

    int lod_item_index_ = 0;
    int part_item_index_ = 0;
    int anim_item_index_ = 0;
};

}
