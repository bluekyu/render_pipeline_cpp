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

#include "render_pipeline/rpcore/util/text3d.hpp"

#include <map>

#include <materialAttrib.h>
#include <textNode.h>
#include <dynamicTextFont.h>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rpcore/loader.hpp"
#include "render_pipeline/rpcore/util/rpmaterial.hpp"

namespace rpcore {

static const std::map<std::string, ::TextNode::Alignment> text_align_map ={
    {"left", ::TextNode::Alignment::A_left},
    {"right", ::TextNode::Alignment::A_right},
    {"center", ::TextNode::Alignment::A_center},
    {"boxed_left", ::TextNode::Alignment::A_boxed_left},
    {"boxed_right", ::TextNode::Alignment::A_boxed_right},
    {"boxed_center", ::TextNode::Alignment::A_boxed_center},
};

class Text3D::Impl
{
public:
    ::TextNode* node_;
    NodePath nodepath_;
};

const float Text3D::Default::pixel_size = 16;
const LColor Text3D::Default::color(1);
const std::string Text3D::Default::align("left");
const Filename Text3D::Default::font = "/$$rp/data/font/Roboto-Bold.ttf";

Text3D::Text3D(const std::string& node_name, NodePath parent, float pixel_size,
    const LVecBase3& pos, const LColor& color, const std::string& align, const Filename& font,
    const std::string& text): impl_(std::make_unique<Impl>())
{
    PT(::TextNode) node = new ::TextNode(node_name);
    node->set_text(text);
    try
    {
        node->set_align(text_align_map.at(align));
    }
    catch (...)
    {
        RPObject::global_error("Text3D", fmt::format("Invalid align ({}).", align));
        node->set_align(text_align_map.at("left"));
    }

    if (parent.is_empty())
        parent = Globals::base->get_render();

    impl_->nodepath_ = parent.attach_new_node(node);
    impl_->nodepath_.set_pos(pos);

    DynamicTextFont* dfont = DCAST(DynamicTextFont, RPLoader::load_font(font));
    //dfont->set_outline(LColor(0, 0, 0, 0.78), 1.6, 0.37);
    dfont->set_outline(LColor(0, 0, 0, 1), 1.6, 0.37);
    dfont->set_scale_factor(1.0);
    dfont->set_texture_margin(int(pixel_size / 4.0 * 2.0));
    dfont->set_bg(LColor(0, 0, 0, 0));
    node->set_font(dfont);
    set_pixel_size(pixel_size);

    impl_->node_ = node;

    set_color(color);
}

Text3D::~Text3D() = default;

NodePath Text3D::get_np() const
{
    return impl_->nodepath_;
}

std::string Text3D::get_text() const
{
    return impl_->node_->get_text();
}

void Text3D::set_text(const std::string& text)
{
    return impl_->node_->set_text(text);
}

LColor Text3D::get_color() const
{
    CPT(RenderState) state = impl_->node_->get_state();

    if (state->has_attrib(MaterialAttrib::get_class_slot()))
    {
        return DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_slot()))->get_material()->get_base_color();
    }
    else
    {
        return LColor(0, 0, 0, 1);
    }
}

void Text3D::set_color(const LColor& color)
{
    CPT(RenderState) state = impl_->node_->get_state();

    RPMaterial mat;
    if (state->has_attrib(MaterialAttrib::get_class_slot()))
        mat = RPMaterial(DCAST(MaterialAttrib, state->get_attrib(MaterialAttrib::get_class_slot()))->get_material());

    mat.set_base_color(color);
    impl_->node_->set_state(state->set_attrib(MaterialAttrib::make(mat.get_material())));
}

void Text3D::set_pixel_size(const LVecBase3& size)
{
    impl_->nodepath_.set_scale(size * 2.0f / float(Globals::native_resolution.get_y()));
}

void Text3D::set_pixel_size(PN_stdfloat sx, PN_stdfloat sy, PN_stdfloat sz)
{
    impl_->nodepath_.set_scale(LVecBase3(sx, sy, sz) * 2.0f / float(Globals::native_resolution.get_y()));
}

void Text3D::set_pixel_size(PN_stdfloat size)
{
    impl_->nodepath_.set_scale(size * 2.0f / float(Globals::native_resolution.get_y()));
}

}   // namespace rpcore
