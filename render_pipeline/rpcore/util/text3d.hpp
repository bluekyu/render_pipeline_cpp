#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

/**
 * Interface for the Panda3D TextNode.
 */
class RENDER_PIPELINE_DECL Text3D
{
public:
    struct Parameters
    {
        std::string node_name = "";
        NodePath parent = NodePath();
        float pixel_size = 16.0f;
        LVecBase3 pos = LVecBase3(0);
        LColor color = LColor(1);
        std::string align = "left";
        std::string font = "/$$rp/data/font/Roboto-Bold.ttf";
        std::string text = "";
    };

public:
    /**
     * Constructs a new text node, forwaring the parameters to the internal
     * panda3d implementation
     */
    Text3D(const Parameters& params=Parameters());
    ~Text3D(void);

    /** Returns the node path of the text. */
    NodePath get_np(void) const;

    /** Returns the current text. */
    std::string get_text(void) const;

    /** Sets the current text. */
    void set_text(const std::string& text);

    /** Returns the current text color. */
    LColor get_color(void) const;

    /** Sets the current text color. */
    void set_color(const LColor& color);

    /** Sets the text size in pixels. */
    void set_pixel_size(const LVecBase3& size);

    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat sx, PN_stdfloat sy, PN_stdfloat sz);

    /** @overload set_pixel_size(const LVecBase3&) */
    void set_pixel_size(PN_stdfloat size);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
