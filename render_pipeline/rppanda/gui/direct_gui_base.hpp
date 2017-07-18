/**
 * This is C++ porting codes of direct/src/gui/DirectGuiBase.py
 */

#pragma once

#include <nodePath.h>
#include <pgFrameStyle.h>

#include <unordered_map>

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class PGItem;

namespace rppanda {

/**
 * Base class for all Direct Gui items.  Handles composite widgets and
 * command line argument parsing.
 */
class RENDER_PIPELINE_DECL DirectGuiBase: public DirectObject
{
public:
    DirectGuiBase(void) = default;

    const std::string& get_gui_id(void) const;

    /** Create a component (during construction or later) for this widget. */
    boost::any& create_component(const std::string& component_name, boost::any&& component);

    /**
     * Return a component widget of the megawidget given the
     * component's name.
     */
    boost::any& get_component(const std::string& name);

    bool has_component(const std::string& name) const;

    void remove_component(const std::string& name);

    void bind(const std::string& event_name, EventHandler::EventCallbackFunction* func, void* user_data);

    void unbind(const std::string& event_name);

protected:
    bool f_init_ = true;
    std::string gui_id_ = "guiObject";

private:
    std::unordered_map<std::string, boost::any> component_info_;

public:
    static const std::type_info& get_class_type(void) { return type_handle_; }
    virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
    static const std::type_info& type_handle_;
};

inline bool DirectGuiBase::has_component(const std::string& name) const
{
    return component_info_.find(name) != component_info_.end();
}

inline const std::string& DirectGuiBase::get_gui_id(void) const
{
    return gui_id_;
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL DirectGuiWidget: public DirectGuiBase, public NodePath
{
public:
    /**
     * Options for Direct GUI.
     *
     * DirectGuiWidget use and **hold** these options, so those are NOT just parameters.
     * Options class should be created using shared pointer (std::shared_ptr)
     */
    struct RENDER_PIPELINE_DECL Options
    {
        Options(void);
        virtual ~Options(void) = default;

        int num_states = 1;
        std::vector<int> inverted_frames;
        int sort_order = 0;
        std::string state;
        PGFrameStyle::Type relief;
        LVecBase2 border_width = LVecBase2(0.1f);
        LVecBase2 border_uv_width = LVecBase2(0.1f);
        boost::optional<LVecBase4> frame_size;
        LColor frame_color = LColor(0.8f, 0.8f, 0.8f, 1.0f);
        std::vector<PT(Texture)> frame_texture;
        boost::optional<LVecBase2> frame_visible_scale;
        LVecBase2 pad = LVecBase2(0);

        boost::optional<LVecBase3> pos;
        boost::optional<LVecBase3> hpr;
        boost::optional<LVecBase3> scale;
        boost::optional<LColor> color;

        bool suppress_mouse = true;
        bool suppress_keys = false;
        bool enable_edit = true;
    };

public:
    static bool gui_edit;
    static std::string inactive_init_state;

    DirectGuiWidget(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    void enable_edit(void);
    void disable_edit(void);

    static void edit_start(const Event* ev, void* user_data);
    static void edit_stop(const Event* ev, void* user_data);

    PGFrameStyle::Type get_relief(void) const;
    const LColor& get_frame_color(void) const;
    const std::vector<PT(Texture)>& get_frame_texture(void) const;
    const LVecBase2& get_frame_visible_scale(void) const;
    const LVecBase2& get_border_uv_width(void) const;
    const LVecBase2& get_border_width(void) const;

    const boost::optional<LVecBase4>& get_frame_size(void) const;

    void set_state(const std::string& state);
    void set_state(bool state);

    void reset_frame_size(void);

    void set_frame_size(bool clear_frame=false);
    void set_frame_size(const LVecBase4& frame_size);

    LVecBase4 get_bounds(int state=0);
    float get_width(void) const;
    float get_height(void) const;
    LVecBase2 get_center(void) const;

    PGFrameStyle::Type get_frame_type(int state=0);

    void update_frame_style(void);

    void set_relief(PGFrameStyle::Type relief);
    void set_relief(const std::string& relief_name);
    void set_frame_color(const LColor& frame_color);
    void set_frame_texture(Texture* texture);
    void set_frame_texture(const std::string& texture_path);
    void set_frame_texture(const std::vector<std::string>& texture_paths);
    void set_frame_texture(const std::vector<PT(Texture)>& textures);
    void set_frame_visible_scale(const LVecBase2& scale);
    void set_border_width(const LVecBase2& border_width);
    void set_border_uv_width(const LVecBase2& border_uv_width);

    void print_config(int indent=0);

    PGItem* get_gui_item(void) const;

protected:
    DirectGuiWidget(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const std::type_info& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);
    void frame_initialise_func(void);

    PGItem* const _gui_item;    ///< This is just for access and stored in NodePath
    std::vector<NodePath> _state_node_path;
    std::shared_ptr<Options> _options;

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

    static bool _snap_to_grid;
    static float _grid_spacing;

    LVecBase4 _bounds;
    LPoint3 _ll;
    LPoint3 _ur;
    std::vector<PGFrameStyle> _frame_style;

public:
    static const std::type_info& get_class_type(void) { return _type_handle; }
    virtual const std::type_info& get_type(void) const { return get_class_type(); }

private:
    static const std::type_info& _type_handle;
};

inline const boost::optional<LVecBase4f>& DirectGuiWidget::get_frame_size(void) const
{
    return _options->frame_size;
}

inline PGFrameStyle::Type DirectGuiWidget::get_relief(void) const
{
    return _options->relief;
}

inline const LColor& DirectGuiWidget::get_frame_color(void) const
{
    return _options->frame_color;
}

inline const std::vector<PT(Texture)>& DirectGuiWidget::get_frame_texture(void) const
{
    return _options->frame_texture;
}

inline const LVecBase2& DirectGuiWidget::get_frame_visible_scale(void) const
{
    return _options->frame_visible_scale.get();
}

inline const LVecBase2& DirectGuiWidget::get_border_uv_width(void) const
{
    return _options->border_uv_width;
}

inline const LVecBase2& DirectGuiWidget::get_border_width(void) const
{
    return _options->border_width;
}

inline float DirectGuiWidget::get_width(void) const
{
    return _bounds[1] - _bounds[0];
}

inline float DirectGuiWidget::get_height(void) const
{
    return _bounds[3] - _bounds[2];
}

inline LVecBase2 DirectGuiWidget::get_center(void) const
{
    PN_stdfloat x = _bounds[0] + (_bounds[1] - _bounds[0]) / 2.0f;
    PN_stdfloat y = _bounds[2] + (_bounds[3] + _bounds[2]) / 2.0f;
    return LVecBase2(x, y);
}

inline PGItem* DirectGuiWidget::get_gui_item(void) const
{
    return _gui_item;
}

}
