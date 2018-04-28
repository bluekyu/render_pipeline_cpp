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
 * This is C++ porting codes of direct/src/gui/DirectGuiGlobals.py
 */

#include "render_pipeline/rppanda/gui/direct_gui_globals.hpp"

#include <pgButton.h>
#include <pgSliderBar.h>
#include <pgEntry.h>
#include <keyboardButton.h>
#include <mouseButton.h>
#include <textNode.h>

namespace rppanda {

const std::unordered_map<std::string, PGFrameStyle::Type> frame_style_dict = {
    { "flat", DGG_FLAT },
    { "raised", DGG_RAISED },
    { "sunken", DGG_SUNKEN },
    { "groove", DGG_GROOVE },
    { "ridge", DGG_RIDGE },
    { "texture_border", DGG_TEXTUREBORDER },
};

const std::string DGG_ENTER = PGButton::get_enter_prefix();
const std::string DGG_EXIT = PGButton::get_exit_prefix();
const std::string DGG_WITHIN = PGButton::get_within_prefix();
const std::string DGG_WITHOUT = PGButton::get_without_prefix();

const std::string DGG_B1CLICK = PGButton::get_click_prefix() + MouseButton::one().get_name() + "-";
const std::string DGG_B2CLICK = PGButton::get_click_prefix() + MouseButton::two().get_name() + "-";
const std::string DGG_B3CLICK = PGButton::get_click_prefix() + MouseButton::three().get_name() + "-";
const std::string DGG_B1PRESS = PGButton::get_press_prefix() + MouseButton::one().get_name() + "-";
const std::string DGG_B2PRESS = PGButton::get_press_prefix() + MouseButton::two().get_name() + "-";
const std::string DGG_B3PRESS = PGButton::get_press_prefix() + MouseButton::three().get_name() + "-";
const std::string DGG_B1RELEASE = PGButton::get_release_prefix() + MouseButton::one().get_name() + "-";
const std::string DGG_B2RELEASE = PGButton::get_release_prefix() + MouseButton::two().get_name() + "-";
const std::string DGG_B3RELEASE = PGButton::get_release_prefix() + MouseButton::three().get_name() + "-";

const std::string DGG_OVERFLOW = PGEntry::get_overflow_prefix();
const std::string DGG_ACCEPT = PGEntry::get_accept_prefix() + KeyboardButton::enter().get_name() + "-";
const std::string DGG_ACCEPTFAILED = PGEntry::get_accept_failed_prefix() + KeyboardButton::enter().get_name() + "-";
const std::string DGG_TYPE = PGEntry::get_type_prefix();
const std::string DGG_ERASE = PGEntry::get_erase_prefix();
const std::string DGG_CURSORMOVE = PGEntry::get_cursormove_prefix();

const std::string DGG_ADJUST = PGSliderBar::get_adjust_prefix();

TextFont* get_default_font()
{
    return TextNode::get_default_font();
}

}
