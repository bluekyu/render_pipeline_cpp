#pragma once

#include <nodePath.h>

#include <render_pipeline/rppanda/showbase/direct_object.h>

class GraphicsOutput;
class Camera;
class Lens;
class GraphicsEngine;
class PandaFramework;
class WindowFramework;
class MouseWatcher;
class AsyncTaskManager;

namespace rppanda {

class RPCPP_DECL ShowBase: public DirectObject
{
public:
    ShowBase(PandaFramework* framework, WindowFramework* window_framework);
    ~ShowBase(void);

    static ShowBase* get_global_ptr(void);

    PandaFramework* get_panda_framework(void) const;
    WindowFramework* get_window_framework(void) const;

    GraphicsEngine* get_graphics_engine(void) const;
    GraphicsWindow* get_win(void) const;

    NodePath get_render(void) const;
    NodePath get_render_2d(void) const;
    NodePath get_aspect_2d(void) const;
    NodePath get_pixel_2d(void) const;
    NodePath get_render_2dp(void) const;
    NodePath get_pixel_2dp(void) const;
    float get_config_aspect_ratio(void) const;

    /**
     * Returns the actual aspect ratio of the indicated (or main
     * window), or the default aspect ratio if there is not yet a
     * main window.
     */
    float get_aspect_ratio(GraphicsOutput* win=nullptr) const;

    /*
     * Returns the actual size of the indicated(or main
     * window), or the default size if there is not yet a
     * main window.
     */
    const LVecBase2i& get_size(GraphicsOutput* win=nullptr) const;

    /**
     * This is the NodePath that should be used to manipulate the camera.
     * This is the node to which the default camera is attached.
     */
     ///@{

     /**
      * self.camera is the parent node of all cameras: a node that
      * we can move around to move all cameras as a group.
      */
    NodePath get_camera(void) const;
    ///@}

    /**
     * This is a NodePath pointing to the Camera object set up for the 3D scene.
     * This is usually a child of get_camera().
     */
    NodePath get_cam(int index=0) const;

    /** Convenience accessor for base.get_cam().node(). */
    Camera* get_cam_node(int index=0) const;

    /** Convenience accessor for base.get_cam_node()->get_lens(). */
    Lens* get_cam_lens(int cam_index=0, int lens_index=0) const;

    MouseWatcher* get_mouse_watcher_node(void) const;
    NodePath get_button_thrower(void) const;

    const NodePath& get_data_root(void) const;
    PandaNode* get_data_root_node(void) const;

    AsyncTaskManager* get_task_mgr(void) const;

    bool open_default_window(void);

    /**
     * Creates the initial, main window for the application, and sets
     * up the mouse and render2d structures appropriately for it.  If
     * this method is called a second time, it will close the
     * previous main window and open a new one, preserving the lens
     * properties in base.camLens.
     * 
     * The return value is true on success, or false on failure (in
     * which case base.win may be either None, or the previous,
     * closed window).
     */
    void open_main_window(void);

    /**
     * Creates a render2d scene graph, the secondary scene graph for
     * 2-d objects and gui elements that are superimposed over the
     * 2-d and 3-d geometry in the window.
     */
    void setup_render_2dp(void);

    void setup_render(void);

    NodePath make_camera2dp(GraphicsWindow* win, int sort=20,
        const LVecBase4f& display_region=LVecBase4f(0, 1, 0, 1),
        const LVecBase4f& coords=LVecBase4f(-1, 1, -1, 1),
        Lens* lens=nullptr, const std::string& camera_name="");

    void setup_data_graph(void);

    void setup_mouse(void);
    void setup_mouse_cb(void);

    void toggle_backface(void);
    void backface_culling_on(void);
    void backface_culling_off(void);

    void toggle_wireframe(void);
    void wireframe_on(void);
    void wireframe_off(void);

    void restart(void);

    void shutdown(void);

    void disable_mouse(void);

    void use_trackball(void);

    void create_base_audio_managers(void);

    /**
     * enable_music/enable_sound_effects are meant to be called in response
     * to a user request so sfxActive/musicActive represent how things
     * *should* be, regardless of App/OS/HW state.
     */
    void enable_music(bool enable);

    /**
     * enable_all_audio and disable_all_audio allow a programmable global override-off
     * for current audio settings.  they're meant to be called when app
     * loses audio focus (switched out), so we can turn off sound without
     * affecting internal sfxActive/musicActive sound settings, so things
     * come back ok when the app is switched back to
     */
    void disable_all_audio(void);

    /** @see disable_all_audio(void) */
    void enable_all_audio(void);

protected:
    GraphicsEngine* graphics_engine = nullptr;
    GraphicsWindow* win = nullptr;

    std::string window_type;
    bool require_window;

    bool want_render_2dp;

    float config_aspect_ratio;

    NodePath render_2dp;
    NodePath aspect_2dp;
    NodePath pixel_2dp;

    NodePath camera2dp;

    float a2dp_top;
    float a2dp_bottom;
    float a2dp_left;
    float a2dp_right;

    NodePath a2dp_top_center;
    NodePath a2dp_bottom_center;
    NodePath a2dp_left_center;
    NodePath a2dp_right_center;

    NodePath a2dp_top_left;
    NodePath a2dp_top_right;
    NodePath a2dp_bottom_left;
    NodePath a2dp_bottom_right;

    bool backface_culling_enabled;
    bool wireframe_enabled;

    NodePath mouse_watcher;
    MouseWatcher* mouse_watcher_node;

private:
    static AsyncTask::DoneStatus ival_loop(GenericAsyncTask *task, void *user_data);

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ************************************************************************************************
inline GraphicsEngine* ShowBase::get_graphics_engine(void) const
{
    return graphics_engine;
}

inline GraphicsWindow* ShowBase::get_win(void) const
{
    return win;
}

inline NodePath ShowBase::get_render_2dp(void) const
{
    return render_2dp;
}

inline NodePath ShowBase::get_pixel_2dp(void) const
{
    return pixel_2dp;
}

inline float ShowBase::get_config_aspect_ratio(void) const
{
    return config_aspect_ratio;
}

inline MouseWatcher* ShowBase::get_mouse_watcher_node(void) const
{
    return mouse_watcher_node;
}

inline void ShowBase::setup_render(void)
{
    // C++ sets already render node.
    //self.render.setAttrib(RescaleNormalAttrib.makeDefault())
    //self.render.setTwoSided(0)

    NodePath render = get_render();
    backface_culling_enabled = render.get_two_sided();
    //textureEnabled = 1;
    wireframe_enabled = render.get_render_mode() == RenderModeAttrib::M_wireframe;
}

inline void ShowBase::toggle_backface(void)
{
    if (backface_culling_enabled)
        backface_culling_off();
    else
        backface_culling_on();
}

inline void ShowBase::backface_culling_on(void)
{
    if (!backface_culling_enabled)
        get_render().set_two_sided(false);
    backface_culling_enabled = true;
}

inline void ShowBase::backface_culling_off(void)
{
    if (!backface_culling_enabled)
        get_render().set_two_sided(true);
    backface_culling_enabled = false;
}

inline void ShowBase::toggle_wireframe(void)
{
    if (wireframe_enabled)
        wireframe_off();
    else
        wireframe_on();
}

inline void ShowBase::wireframe_on(void)
{
    NodePath render = get_render();
    render.set_render_mode_wireframe(100);
    render.set_two_sided(1);
    wireframe_enabled = true;
}

inline void ShowBase::wireframe_off(void)
{
    NodePath render = get_render();
    render.clear_render_mode();
    render.set_two_sided(!backface_culling_enabled);
    wireframe_enabled = false;
}

}
