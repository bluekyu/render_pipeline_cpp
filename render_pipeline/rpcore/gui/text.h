#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.h>

class TextFont;

namespace rpcore {

/** Simple wrapper around OnscreenText, providing a simpler interface. */
class Text: public RPObject
{
public:
    struct Parameters
    {
        std::string text = "";
        NodePath parent = NodePath();
        float x = 0.0f;
        float y = 0.0f;
        float size = 10.0f;
        std::string align = "left";
        LVecBase3 color = LVecBase3(1);
        bool may_change = false;
        TextFont* font = nullptr;
    };

public:
    /**
     * Constructs a new text. The parameters are almost equal to the
     * parameters of OnscreenText.
     */
    Text(const Parameters& params=Parameters());

    rppanda::OnscreenText get_node(void) const;

    /**
     * Changes the text, remember to pass may_change to the constructor,
     * otherwise this method does not work.
     */
    void set_text(const std::string& text);

    const LVecBase2f& get_initial_pos(void) const;

private:
    LVecBase2f _initial_pos;
    rppanda::OnscreenText _node;
};

inline rppanda::OnscreenText Text::get_node(void) const
{
    return _node;
}

inline void Text::set_text(const std::string& text)
{
    _node.set_text(text);
}

inline const LVecBase2f& Text::get_initial_pos(void) const
{
    return _initial_pos;
}

}
