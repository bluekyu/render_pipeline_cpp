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

/**
 * This is C++ porting codes of direct/src/gui/DirectGuiGlobals.py
 */

#pragma once

#include <pgFrameStyle.h>

#include <string>
#include <unordered_map>

#include <render_pipeline/rpcore/config.hpp>

class TextFont;

namespace rppanda {

// Mouse buttons
static const int LMB = 0;
static const int MMB = 1;
static const int RMB = 2;

// Widget state
static const std::string NORMAL("normal");
static const std::string DISABLED("disabled");

static const PGFrameStyle::Type FLAT = PGFrameStyle::Type::T_flat;
static const PGFrameStyle::Type RAISED = PGFrameStyle::Type::T_bevel_out;
static const PGFrameStyle::Type SUNKEN = PGFrameStyle::Type::T_bevel_in;
static const PGFrameStyle::Type GROOVE = PGFrameStyle::Type::T_groove;
static const PGFrameStyle::Type RIDGE = PGFrameStyle::Type::T_ridge;
static const PGFrameStyle::Type TEXTUREBORDER = PGFrameStyle::Type::T_texture_border;

RENDER_PIPELINE_DECL extern const std::unordered_map<std::string, PGFrameStyle::Type> FrameStyleDict;

// Orientation of DirectSlider and DirectScrollBar
static const std::string HORIZONTAL("horizontal");
static const std::string VERTICAL("vertical");
static const std::string VERTICAL_INVERTED("vertical_inverted");

// User can bind commands to these gui events
static const std::string DESTROY("destroy-");
static const std::string PRINT("print-");

RENDER_PIPELINE_DECL extern const std::string ENTER;
RENDER_PIPELINE_DECL extern const std::string EXIT;
RENDER_PIPELINE_DECL extern const std::string WITHIN;
RENDER_PIPELINE_DECL extern const std::string WITHOUT;

RENDER_PIPELINE_DECL extern const std::string B1CLICK;
RENDER_PIPELINE_DECL extern const std::string B2CLICK;
RENDER_PIPELINE_DECL extern const std::string B3CLICK;
RENDER_PIPELINE_DECL extern const std::string B1PRESS;
RENDER_PIPELINE_DECL extern const std::string B2PRESS;
RENDER_PIPELINE_DECL extern const std::string B3PRESS;
RENDER_PIPELINE_DECL extern const std::string B1RELEASE;
RENDER_PIPELINE_DECL extern const std::string B2RELEASE;
RENDER_PIPELINE_DECL extern const std::string B3RELEASE;
// For DirectSlider and DirectScrollBar widgets
RENDER_PIPELINE_DECL extern const std::string ADJUST;

// For setting the sorting order of a widget's visible components
static const int IMAGE_SORT_INDEX = 10;
static const int GEOM_SORT_INDEX = 20;
static const int TEXT_SORT_INDEX = 30;

RENDER_PIPELINE_DECL extern TextFont* get_default_font(void);

}
