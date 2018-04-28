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
 * This is C++ porting codes of direct/src/gui/DirectFrame.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>

class PGEntry;

namespace rppanda {

/**
 * Create a DirectGuiWidget which responds to keyboard buttons.
 */
class RENDER_PIPELINE_DECL DirectEntry : public DirectFrame
{
public:
    static const std::vector<std::string> ALLOW_CAP_NAME_PREFIXES;
    static const std::vector<std::string> FORCE_CAP_NAME_PREFIXES;

    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options : public DirectFrame::Options
    {
        Options();

        TextFont* entry_font = nullptr;

        float width = 10;
        int num_lines = 1;
        bool focus = false;
        bool cursor_keys = true;
        bool obscured = false;
        // Setting backgroundFocus allows the entry box to get keyboard
        bool background_focus = false;

        std::string initial_text;

        // Command to be called on hitting Enter
        std::function<void(const std::string&)> command;
        // bind "extraArgs" to std::function

        // Command to be called when enter is hit but we fail to submit
        std::function<void(const std::string&)> failed_command;

        // commands to be called when focus is gained or lost
        std::function<void()> focus_in_command;
        std::function<void()> focus_out_command;

        bool auto_capitalize = false;
        std::vector<std::string> auto_capitalize_allow_prefixes;
        std::vector<std::string> auto_capitalize_force_prefixes;
    };

public:
    DirectEntry(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());
    ~DirectEntry();

    ALLOC_DELETED_CHAIN(DirectEntry);

    PGEntry* get_gui_item() const;

    void setup();

    void prepare_width(float width);
    void update_width();
    void set_width(float width);

    void prepare_num_lines(int num_lines);
    void update_num_lines();
    void set_num_lines(int num_lines);

    void prepare_focus(bool focus);
    void update_focus();
    void set_focus(bool focus);

    void prepare_cursor_keys_active(bool cursor_keys);
    void update_cursor_keys_active();
    void set_cursor_keys_active(bool cursor_keys);

    void update_obscure_mode();

    void update_background_focus();

    virtual void command_func();
    virtual void failed_command_func();

    void prepare_auto_capitalize_func(bool flag);
    void update_auto_capitalize_func();
    void set_auto_capitalize_func(bool flag);

    virtual void focus_in_command_func();
    virtual void focus_out_command_func();

    /**
     * Changes the text currently showing in the typable region;
     * does not change the current cursor position.  Also see
     * enter_text().
     */
    void set(const std::string& text);

    /**
     * Returns the text currently showing in the typable region.
     * If plain is True, the returned text will not include any
     * formatting characters like nested color-change codes.
     */
    std::string get(bool plane = false) const;

    int get_cursor_position() const;
    void set_cursor_position(int pos);

    int get_num_characters() const;

    /** sets the entry's text, and moves the cursor to the end. */
    void enter_text(const std::string& text);

    LVecBase4 get_bounds(int state = 0) override;

    TextFont* get_font() const;

protected:
    DirectEntry(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

    virtual void handle_typing();
    virtual void handle_erasing();
    virtual void auto_capitalize();

    OnscreenText onscreen_text_;
    PT(DirectObject) auto_cap_listener_;

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline void DirectEntry::set_width(float width)
{
    prepare_width(width);
    update_width();
}

inline void DirectEntry::set_num_lines(int num_lines)
{
    prepare_num_lines(num_lines);
    update_num_lines();
}

inline void DirectEntry::set_focus(bool focus)
{
    prepare_focus(focus);
    update_focus();
}

inline void DirectEntry::set_cursor_keys_active(bool cursor_keys)
{
    prepare_cursor_keys_active(cursor_keys);
    update_cursor_keys_active();
}

inline void DirectEntry::set_auto_capitalize_func(bool flag)
{
    prepare_auto_capitalize_func(flag);
    update_auto_capitalize_func();
}

inline TextFont* DirectEntry::get_font() const
{
    return onscreen_text_.get_font();
}

inline TypeHandle DirectEntry::get_class_type()
{
    return type_handle_;
}

inline void DirectEntry::init_type()
{
    DirectFrame::init_type();
    register_type(type_handle_, "rppanda::DirectEntry", DirectFrame::get_class_type());
}

inline TypeHandle DirectEntry::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectEntry::force_init_type()
{
    init_type();
    return get_class_type();
}

}
