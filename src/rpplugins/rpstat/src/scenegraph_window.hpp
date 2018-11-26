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

#include <nodePath.h>

#include "window_interface.hpp"

namespace rppanda {
class Actor;
}

namespace rpplugins {

class MessageDialog;
class ImportModelDialog;

class ScenegraphWindow : public WindowInterface
{
public:
    static constexpr const char* NODE_SELECTED_EVENT_NAME = "rpstat-scenegraph-selected";
    static constexpr const char* CHANGE_SELECTED_NODE_EVENT_NAME = "rpstat-scenegraph-change-selected-node";

public:
    ScenegraphWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline);
    ~ScenegraphWindow() override;

    void draw() final;
    void draw_contents() final;

    void change_selected_nodepath(NodePath np);

    rppanda::Actor* get_actor(NodePath actor) const;
    bool is_actor(NodePath actor) const;
    void add_actor(PT(rppanda::Actor) actor);
    void remove_actor(rppanda::Actor* actor);
    void remove_actor(NodePath actor);

private:
    void draw_nodepath(NodePath np);
    void draw_nodepath_context_menu(NodePath np);
    void draw_geomnode(GeomNode* node);
    void draw_gizmo();
    void draw_import_model();
    void draw_import_actor();

    NodePath selected_np_;
    NodePath payload_np_;
    const Geom* selected_geom_ = nullptr;

    NodePath root_;

    int gizmo_op_ = 0;

    std::map<NodePath, PT(rppanda::Actor)> actor_map_;

    std::unique_ptr<ImportModelDialog> import_model_dialog_;
    std::unique_ptr<ImportModelDialog> import_actor_dialog_;

    std::unique_ptr<MessageDialog> message_dialog_;
    NodePath dialog_np_;

    NodePath will_remove_np_;
};

}
