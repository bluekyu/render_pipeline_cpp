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
 * This is C++ porting codes of direct/src/gui/DirectScrollFrame.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_frame.hpp>
#include <render_pipeline/rppanda/gui/direct_scroll_bar.hpp>

class PGScrollFrame;

namespace rppanda {

class RENDER_PIPELINE_DECL DirectScrolledFrame : public DirectFrame
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options : public DirectFrame::Options
    {
        Options();

        LVecBase4f canvas_size = LVecBase4f(-1, 1, -1, 1);
        bool manage_scroll_bars = true;
        bool auto_hide_scroll_bars = true;
        float scroll_bar_width = 0.08f;

        std::shared_ptr<DirectScrollBar::Options> vertical_scroll_options;
        std::shared_ptr<DirectScrollBar::Options> horizontal_scroll_options;
    };

public:
    DirectScrolledFrame(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());
    virtual ~DirectScrolledFrame();

    ALLOC_DELETED_CHAIN(DirectScrolledFrame);

    PGScrollFrame* get_gui_item() const;

    NodePath get_canvas();
    DirectScrollBar* get_vertical_scroll() const;
    DirectScrollBar* get_horizontal_scroll() const;

    const LVecBase4f& get_canvas_size() const;
    bool get_manage_scroll_bars() const;
    bool get_auto_hide_scroll_bars() const;

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
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline NodePath DirectScrolledFrame::get_canvas()
{
    return _canvas;
}

inline DirectScrollBar* DirectScrolledFrame::get_vertical_scroll() const
{
    return _vertical_scroll;
}

inline DirectScrollBar* DirectScrolledFrame::get_horizontal_scroll() const
{
    return _horizontal_scroll;
}

inline const LVecBase4f& DirectScrolledFrame::get_canvas_size() const
{
    return static_cast<Options*>(options_.get())->canvas_size;
}

inline bool DirectScrolledFrame::get_manage_scroll_bars() const
{
    return static_cast<Options*>(options_.get())->manage_scroll_bars;
}

inline bool DirectScrolledFrame::get_auto_hide_scroll_bars() const
{
    return static_cast<Options*>(options_.get())->auto_hide_scroll_bars;
}

inline TypeHandle DirectScrolledFrame::get_class_type()
{
    return type_handle_;
}

inline void DirectScrolledFrame::init_type()
{
    DirectFrame::init_type();
    register_type(type_handle_, "rppanda::DirectScrolledFrame", DirectFrame::get_class_type());
}

inline TypeHandle DirectScrolledFrame::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectScrolledFrame::force_init_type()
{
    init_type();
    return get_class_type();
}

}
