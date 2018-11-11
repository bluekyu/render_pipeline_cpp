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

#include "window_interface.hpp"

class AnimControl;

namespace rpplugins {

class AnimControlWindow : public WindowInterface
{
public:
    static std::pair<AnimControlWindow*, bool> create_window(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, AnimControl* control);
    static void remove_window(AnimControlWindow* window);

public:
    void draw() final;
    void draw_contents() final;

    void reset();

private:
    AnimControlWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline, AnimControl* control);

    static size_t global_id_;
    static std::unordered_map<AnimControl*, AnimControlWindow*> holder_;

    AnimControl* control_;

    float from_frame_ = 0;
    float to_frame_ = 0;
    bool restart_ = false;
};

}
