#include "render_pipeline/rpcore/util/text3d.hpp"

#include <map>

#include <materialAttrib.h>
#include <textNode.h>
#include <dynamicTextFont.h>

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

Text3D::Text3D(const Parameters& params): impl_(std::make_unique<Impl>())
{
    PT(::TextNode) node = new ::TextNode(params.node_name);
    node->set_text(params.text);
    try
    {
        node->set_align(text_align_map.at(params.align));
    }
    catch (...)
    {
        std::cout << "Invalid align (" << params.align << ").";
        node->set_align(text_align_map.at("left"));
    }

    NodePath parent = params.parent;
    if (parent.is_empty())
        parent = Globals::base->get_render();

    impl_->nodepath_ = parent.attach_new_node(node);
    impl_->nodepath_.set_pos(params.pos);

    DynamicTextFont* dfont = DCAST(DynamicTextFont, RPLoader::load_font(params.font));
    //dfont->set_outline(LColor(0, 0, 0, 0.78), 1.6, 0.37);
    dfont->set_outline(LColor(0, 0, 0, 1), 1.6, 0.37);
    dfont->set_scale_factor(1.0);
    dfont->set_texture_margin(int(params.pixel_size / 4.0 * 2.0));
    dfont->set_bg(LColor(0, 0, 0, 0));
    node->set_font(dfont);
    set_pixel_size(params.pixel_size);

    impl_->node_ = node;

    set_color(params.color);
}

Text3D::~Text3D(void) = default;

NodePath Text3D::get_np(void) const
{
    return impl_->nodepath_;
}

std::string Text3D::get_text(void) const
{
    return impl_->node_->get_text();
}

void Text3D::set_text(const std::string& text)
{
    return impl_->node_->set_text(text);
}

LColor Text3D::get_color(void) const
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
