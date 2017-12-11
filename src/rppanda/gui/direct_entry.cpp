/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/gui/DirectEntry.py
 */

#include "render_pipeline/rppanda/gui/direct_entry.hpp"

#include <cctype>

#include <pgEntry.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

namespace rppanda {

const std::vector<std::string> DirectEntry::allow_cap_name_prefixes_ = {
    "Al", "Ap", "Ben", "De", "Del", "Della", "Delle", "Der", "Di", "Du",
    "El", "Fitz", "La", "Las", "Le", "Les", "Lo", "Los",
    "Mac", "St", "Te", "Ten", "Van", "Von",
};

const std::vector<std::string> DirectEntry::force_cap_name_prefixes_ = {
    "D'", "DeLa", "Dell'", "L'", "M'", "Mc", "O'",
};

TypeHandle DirectEntry::type_handle_;

DirectEntry::Options::Options()
{
    num_states = 3;
    state = NORMAL;

    auto_capitalize_allow_prefixes = DirectEntry::allow_cap_name_prefixes_;
    auto_capitalize_force_prefixes = DirectEntry::force_cap_name_prefixes_;
}

DirectEntry::DirectEntry(NodePath parent, const std::shared_ptr<Options>& options): DirectEntry(new PGEntry(""), parent, options, get_class_type())
{
}

DirectEntry::DirectEntry(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle):
    DirectFrame(gui_item, parent, define_options(options), type_handle)
{
    TextFont* font;
    if (options->entry_font)
        font = options->entry_font;
    else
        font = get_default_font();

#if !defined(_MSC_VER) || _MSC_VER >= 1900
    // Create Text Node Component
    onscreen_text_ = OnscreenText(
        "", OnscreenText::Default::style, LVecBase2(0), 0, LVecBase2(1),
        {}, {}, {}, OnscreenText::Default::shadow_offset, {},
        TextNode::A_left, {}, {}, false, font, rppanda::ShowBase::get_global_ptr()->get_hidden(), 0,
        // Don't get rid of the text node
        true);
#else
    onscreen_text_ = OnscreenText(
        "", OnscreenText::Default::style, LVecBase2(0), 0, LVecBase2(1),
        boost::none, boost::none, boost::none, OnscreenText::Default::shadow_offset, boost::none,
        TextNode::A_left, boost::none, boost::none, false, font, rppanda::ShowBase::get_global_ptr()->get_hidden(), 0,
        // Don't get rid of the text node
        true);
#endif

    create_component("text", boost::any(onscreen_text_));

    // We can get rid of the node path since we're just using the
    // onscreenText as an easy way to access a text node as a
    // component
    onscreen_text_.remove_node();

    // Bind command function
    bind(DGG_ACCEPT, [this](const Event* ev) { command_func(); });
    bind(DGG_ACCEPTFAILED, [this](const Event* ev) { failed_command_func(); });

    accept(get_gui_item()->get_focus_in_event(), [this](const Event* ev) { focus_in_command_func(); });
    accept(get_gui_item()->get_focus_out_event(), [this](const Event* ev) { focus_out_command_func(); });

    // listen for auto - capitalize events on a separate object to prevent
    // clashing with other parts of the system
    auto_cap_listener_ = new DirectObject;

    // Call option initialization functions
    if (is_exact_type(type_handle))
    {
        initialise_options(options);
        frame_initialise_func();
    }

    // Update TextNodes for each state
    for (int i = 0; i < options->num_states; ++i)
        get_gui_item()->set_text_def(i, DCAST(TextNode, onscreen_text_.get_text_node()));

    // Now we should call setup() again to make sure it has the
    // right font def.
    setup();

    // Update initial text
    if (!options->initial_text.empty())
        enter_text(options->initial_text);
}

DirectEntry::~DirectEntry() = default;

PGEntry* DirectEntry::get_gui_item() const
{
    return DCAST(PGEntry, _gui_item);
}

void DirectEntry::setup()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    get_gui_item()->setup_minimal(options->width, options->num_lines);
}

void DirectEntry::prepare_width(float width)
{
    std::dynamic_pointer_cast<Options>(_options)->width = width;
}

void DirectEntry::update_width()
{
    get_gui_item()->set_max_width(std::dynamic_pointer_cast<Options>(_options)->width);
}

void DirectEntry::prepare_num_lines(int num_lines)
{
    std::dynamic_pointer_cast<Options>(_options)->num_lines = num_lines;
}

void DirectEntry::update_num_lines()
{
    get_gui_item()->set_num_lines(std::dynamic_pointer_cast<Options>(_options)->num_lines);
}

void DirectEntry::prepare_focus(bool focus)
{
    std::dynamic_pointer_cast<Options>(_options)->focus = focus;
}

void DirectEntry::update_focus()
{
    get_gui_item()->set_focus(std::dynamic_pointer_cast<Options>(_options)->focus);
}

void DirectEntry::prepare_cursor_keys_active(bool cursor_keys)
{
    std::dynamic_pointer_cast<Options>(_options)->cursor_keys = cursor_keys;
}

void DirectEntry::update_cursor_keys_active()
{
    get_gui_item()->set_cursor_keys_active(std::dynamic_pointer_cast<Options>(_options)->cursor_keys);
}

void DirectEntry::update_obscure_mode()
{
    get_gui_item()->set_obscure_mode(std::dynamic_pointer_cast<Options>(_options)->obscured);
}

void DirectEntry::update_background_focus()
{
    get_gui_item()->set_background_focus(std::dynamic_pointer_cast<Options>(_options)->background_focus);
}

void DirectEntry::command_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    if (options->command)
        options->command(get());
}

void DirectEntry::failed_command_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    if (options->failed_command)
        options->failed_command(get());
}

void DirectEntry::prepare_auto_capitalize_func(bool flag)
{
    std::dynamic_pointer_cast<Options>(_options)->auto_capitalize = flag;
}

void DirectEntry::update_auto_capitalize_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    const auto& gui_item = get_gui_item();
    if (options->auto_capitalize)
    {
        auto_cap_listener_->accept(gui_item->get_type_event(), [this](const Event*) { handle_typing(); });
        auto_cap_listener_->accept(gui_item->get_erase_event(), [this](const Event*) { handle_erasing(); });
    }
    else
    {
        auto_cap_listener_->ignore(gui_item->get_type_event());
        auto_cap_listener_->ignore(gui_item->get_erase_event());
    }
}

void DirectEntry::focus_in_command_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    if (options->focus_in_command)
        options->focus_in_command();
    if (options->auto_capitalize)
    {
        const auto& gui_item = get_gui_item();
        accept(gui_item->get_type_event(), [this](const Event*) { handle_typing(); });
        accept(gui_item->get_erase_event(), [this](const Event*) { handle_erasing(); });
    }
}

void DirectEntry::focus_out_command_func()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    if (options->focus_out_command)
        options->focus_out_command();
    if (options->auto_capitalize)
    {
        const auto& gui_item = get_gui_item();
        ignore(gui_item->get_type_event());
        ignore(gui_item->get_erase_event());
    }
}

void DirectEntry::set(const std::string& text)
{
    get_gui_item()->set_text(text);
}

std::string DirectEntry::get(bool plane) const
{
    if (plane)
        return get_gui_item()->get_plain_text();
    else
        return get_gui_item()->get_text();
}

void DirectEntry::set_cursor_position(int pos)
{
    auto gui_item = get_gui_item();
    if (pos < 0)
        gui_item->set_cursor_position(gui_item->get_num_characters() + pos);
    else
        gui_item->set_cursor_position(pos);
}

void DirectEntry::enter_text(const std::string& text)
{
    set(text);
    set_cursor_position(get_gui_item()->get_num_characters());
}

LVecBase4 DirectEntry::get_bounds(int state)
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);

    // Compute the width and height for the entry itself, ignoring
    // geometry etc.
    const auto tn = DCAST(TextNode, onscreen_text_.get_text_node());
    const auto mat = tn->get_transform();
    const auto align = tn->get_align();
    const auto line_height = tn->get_line_height();
    const auto num_lines = options->num_lines;
    const auto width = options->width;

    float left = 0.0f;
    float right = 0.0f;
    switch (align)
    {
    case TextNode::A_left:
        right = width;
        break;

    case TextNode::A_center:
        left = -width / 2.0f;
        right = width / 2.0f;
        break;
    case TextNode::A_right:
        left = -width;
        break;
    default:
        break;
    }

    float bottom = -0.3f * line_height - (line_height * (num_lines - 1));
    float top = line_height;

    ll_.set(left, 0.0, bottom);
    ur_.set(right, 0.0, top);
    ll_ = mat.xform_point(LPoint3::rfu(left, 0.0, bottom));
    ur_ = mat.xform_point(LPoint3::rfu(right, 0.0, top));

    const auto vec_right = LVector3::right();
    const auto vec_up = LVector3::up();
    left = (vec_right[0] * ll_[0]
        + vec_right[1] * ll_[1]
        + vec_right[2] * ll_[2]);
    right = (vec_right[0] * ur_[0]
        + vec_right[1] * ur_[1]
        + vec_right[2] * ur_[2]);
    bottom = (vec_up[0] * ll_[0]
        + vec_up[1] * ll_[1]
        + vec_up[2] * ll_[2]);
    top = (vec_up[0] * ur_[0]
        + vec_up[1] * ur_[1]
        + vec_up[2] * ur_[2]);
    ll_ = LPoint3(left, 0.0, bottom);
    ur_ = LPoint3(right, 0.0, top);

    // Scale bounds to give a pad around graphics.We also want to
    // scale around the border width.
    auto pad = options->pad;
    auto border_width = options->border_width;
    bounds_.set(ll_[0] - pad[0] - border_width[0],
                ur_[0] + pad[0] + border_width[0],
                ll_[2] - pad[1] - border_width[1],
                ur_[2] + pad[1] + border_width[1]);
    return bounds_;
}

const std::shared_ptr<DirectEntry::Options>& DirectEntry::define_options(const std::shared_ptr<Options>& options)
{
    return options;
}

void DirectEntry::initialise_options(const std::shared_ptr<Options>& options)
{
    DirectFrame::initialise_options(options);

    f_init_ = true;
    update_width();
    update_num_lines();
    update_focus();
    update_cursor_keys_active();
    update_obscure_mode();
    update_background_focus();
    update_auto_capitalize_func();
    f_init_ = false;
}

void DirectEntry::handle_typing()
{
    auto_capitalize();
}

void DirectEntry::handle_erasing()
{
    auto_capitalize();
}

void DirectEntry::auto_capitalize()
{
    const auto& options = std::dynamic_pointer_cast<Options>(_options);
    const auto& auto_capitalize_allow_prefixes = options->auto_capitalize_allow_prefixes;
    const auto& auto_capitalize_force_prefixes = options->auto_capitalize_force_prefixes;

    const auto name = get();

    // capitalize each word, allowing for things like McMutton
    std::string cap_name;
    // track each individual word to detect prefixes like Mc
    std::string word_so_far;
    // track whether the previous character was part of a word or not
    bool was_non_word_char = true;

    bool capitalize = false;
    for (auto character: name)
    {
        // test to see if we are between words
        // - Count characters that can't be capitalized as a break between words
        //   This assumes that string.lower and string.upper will return different
        //   values for all unicode letters.
        // - Don't count apostrophes as a break between words
        if (std::tolower(character) == std::toupper(character) && character != '\'')
        {
            // we are between words
            was_non_word_char = true;
        }
        else
        {
            capitalize = false;
            if (was_non_word_char)
            {
                // first letter of a word, capitalize it unconditionally;
                capitalize = true;
            }
            else if (character == std::toupper(character) && auto_capitalize_allow_prefixes.size() &&
                std::find(auto_capitalize_allow_prefixes.begin(), auto_capitalize_allow_prefixes.end(), word_so_far) != auto_capitalize_allow_prefixes.end())
            {
                // first letter after one of the prefixes, allow it to be capitalized
                capitalize = true;
            }
            else if (auto_capitalize_force_prefixes.size() &&
                std::find(auto_capitalize_force_prefixes.begin(), auto_capitalize_force_prefixes.end(), word_so_far) != auto_capitalize_force_prefixes.end())
            {
                // first letter after one of the force prefixes, force it to be capitalized
                capitalize = true;
            }

            if (capitalize)
            {
                // allow this letter to remain capitalized
                character = std::toupper(character);
            }
            else
            {
                character = std::tolower(character);
            }
            word_so_far += character;
            was_non_word_char = false;
        }
        cap_name += character;
    }
    enter_text(cap_name);
}

}
