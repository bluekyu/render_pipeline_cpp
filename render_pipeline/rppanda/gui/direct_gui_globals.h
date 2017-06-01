#pragma once

#include <string>
#include <unordered_map>

#include <pgFrameStyle.h>

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

extern const std::unordered_map<std::string, PGFrameStyle::Type> FrameStyleDict;

// Orientation of DirectSlider and DirectScrollBar
static const std::string HORIZONTAL("horizontal");
static const std::string VERTICAL("vertical");
static const std::string VERTICAL_INVERTED("vertical_inverted");

// User can bind commands to these gui events
static const std::string DESTROY("destroy-");
static const std::string PRINT("print-");

extern const std::string ENTER;
extern const std::string EXIT;
extern const std::string WITHIN;
extern const std::string WITHOUT;

extern const std::string B1CLICK;
extern const std::string B2CLICK;
extern const std::string B3CLICK;
extern const std::string B1PRESS;
extern const std::string B2PRESS;
extern const std::string B3PRESS;
extern const std::string B1RELEASE;
extern const std::string B2RELEASE;
extern const std::string B3RELEASE;
// For DirectSlider and DirectScrollBar widgets
extern const std::string ADJUST;

// For setting the sorting order of a widget's visible components
static const int IMAGE_SORT_INDEX = 10;
static const int GEOM_SORT_INDEX = 20;
static const int TEXT_SORT_INDEX = 30;

extern TextFont* get_default_font(void);

}
