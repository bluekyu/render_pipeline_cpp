#include "rpcore/gui/text_node.hpp"

#include <map>

#include <textNode.h>
#include <dynamicTextFont.h>

#include "render_pipeline/rpcore/globals.h"
#include "render_pipeline/rppanda/showbase/showbase.h"
#include "render_pipeline/rpcore/loader.h"

namespace rpcore {

static const std::map<std::string, ::TextNode::Alignment> text_align_map ={
    {"left", ::TextNode::Alignment::A_left},
    {"right", ::TextNode::Alignment::A_right},
    {"center", ::TextNode::Alignment::A_center},
    {"boxed_left", ::TextNode::Alignment::A_boxed_left},
    {"boxed_right", ::TextNode::Alignment::A_boxed_right},
    {"boxed_center", ::TextNode::Alignment::A_boxed_center},
};

struct TextNode::Impl
{
    ::TextNode* node_;
    NodePath nodepath_;
};

TextNode::TextNode(const Parameters& params): RPObject("TextNode"), impl_(std::make_unique<Impl>())
{
    PT(::TextNode) node = new ::TextNode("FTN");
    node->set_text(params.text);
    node->set_align(text_align_map.at(params.align));
    node->set_text_color(LColorf(params.color, 1.0f));

    NodePath parent = params.parent;
    if (parent.is_empty())
        parent = Globals::base->get_aspect_2d();

    impl_->nodepath_ = parent.attach_new_node(node);
    impl_->nodepath_.set_pos(params.pos.get_x(), 0, params.pos.get_y());

    DynamicTextFont* dfont = DCAST(DynamicTextFont, RPLoader::load_font(params.font));
    //dfont->set_outline(LColor(0, 0, 0, 0.78), 1.6, 0.37);
    dfont->set_outline(LColor(0, 0, 0, 1), 1.6, 0.37);
    dfont->set_scale_factor(1.0);
    dfont->set_texture_margin(int(params.pixel_size / 4.0 * 2.0));
    dfont->set_bg(LColor(0, 0, 0, 0));
    node->set_font(dfont);
    set_pixel_size(params.pixel_size);

    impl_->node_ = node;
}

TextNode::~TextNode(void) = default;

NodePath TextNode::get_np(void) const
{
    return impl_->nodepath_;
}

std::string TextNode::get_text(void) const
{
    return impl_->node_->get_text();
}

void TextNode::set_text(const std::string& text)
{
    return impl_->node_->set_text(text);
}

LColor TextNode::get_color(void) const
{
    return impl_->node_->get_text_color();
}

void TextNode::set_color(const LColor& color)
{
    impl_->node_->set_text_color(color);
}

void TextNode::set_pixel_size(const LVecBase3& size)
{
    impl_->nodepath_.set_scale(size * 2.0f / float(Globals::native_resolution.get_y()));
}

void TextNode::set_pixel_size(PN_stdfloat sx, PN_stdfloat sy, PN_stdfloat sz)
{
    impl_->nodepath_.set_scale(LVecBase3(sx, sy, sz) * 2.0f / float(Globals::native_resolution.get_y()));
}

void TextNode::set_pixel_size(PN_stdfloat size)
{
    impl_->nodepath_.set_scale(size * 2.0f / float(Globals::native_resolution.get_y()));
}

}   // namespace rpcore
