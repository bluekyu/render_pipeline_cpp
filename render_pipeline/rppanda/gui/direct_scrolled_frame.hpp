/**
 * This is C++ porting codes of direct/src/gui/DirectScrollFrame.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>
#include <render_pipeline/rppanda/gui/direct_scroll_bar.hpp>

class PGScrollFrame;

namespace rppanda {

class RENDER_PIPELINE_DECL DirectScrolledFrame: public DirectFrame
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options: public DirectFrame::Options
    {
        Options(void);

        LVecBase4f canvas_size = LVecBase4f(-1, 1, -1, 1);
        bool manage_scroll_bars = true;
        bool auto_hide_scroll_bars = true;
        float scroll_bar_width = 0.08f;

        std::shared_ptr<DirectScrollBar::Options> vertical_scroll_options;
        std::shared_ptr<DirectScrollBar::Options> horizontal_scroll_options;
    };

public:
    DirectScrolledFrame(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    PGScrollFrame* get_gui_item(void) const;

    NodePath get_canvas(void);
    DirectScrollBar* get_vertical_scroll(void) const;
    DirectScrollBar* get_horizontal_scroll(void) const;

    const LVecBase4f& get_canvas_size(void) const;
    bool get_manage_scroll_bars(void) const;
    bool get_auto_hide_scroll_bars(void) const;

    void set_canvas_size(const LVecBase4f& canvas_size);
    void set_manage_scroll_bars(bool manage_scroll_bars);
    void set_auto_hide_scroll_bars(bool auto_hide_scroll_bars);

protected:
    DirectScrolledFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

    NodePath _canvas;

    PT(DirectScrollBar) _vertical_scroll;
    PT(DirectScrollBar) _horizontal_scroll;

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline NodePath DirectScrolledFrame::get_canvas(void)
{
    return _canvas;
}

inline DirectScrollBar* DirectScrolledFrame::get_vertical_scroll(void) const
{
    return _vertical_scroll;
}

inline DirectScrollBar* DirectScrolledFrame::get_horizontal_scroll(void) const
{
    return _horizontal_scroll;
}

inline const LVecBase4f& DirectScrolledFrame::get_canvas_size(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->canvas_size;
}

inline bool DirectScrolledFrame::get_manage_scroll_bars(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->manage_scroll_bars;
}

inline bool DirectScrolledFrame::get_auto_hide_scroll_bars(void) const
{
    return std::dynamic_pointer_cast<Options>(_options)->auto_hide_scroll_bars;
}

inline TypeHandle DirectScrolledFrame::get_class_type(void)
{
    return type_handle_;
}

inline void DirectScrolledFrame::init_type(void)
{
    DirectFrame::init_type();
    register_type(type_handle_, "rppanda::DirectScrolledFrame", DirectFrame::get_class_type());
}

inline TypeHandle DirectScrolledFrame::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle DirectScrolledFrame::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
