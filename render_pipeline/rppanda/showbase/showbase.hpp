#pragma once

#include <nodePath.h>

#include <boost/optional.hpp>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

class GraphicsOutput;
class Camera;
class Lens;
class GraphicsEngine;
class PandaFramework;
class WindowFramework;
class MouseWatcher;
class AsyncTaskManager;
class AudioManager;
class AudioSound;

namespace rppanda {

class RENDER_PIPELINE_DECL ShowBase: public DirectObject
{
public:
    /** Create PandaFramework and open main window, and then initialize ShowBase. */
    ShowBase(int& argc, char**& argv);

    /** Initialize ShowBase with given PandaFramework. */
    ShowBase(PandaFramework* framework);

    ~ShowBase(void);

    static ShowBase* get_global_ptr(void);

    PandaFramework* get_panda_framework(void) const;
    WindowFramework* get_window_framework(void) const;

    GraphicsEngine* get_graphics_engine(void) const;
    GraphicsWindow* get_win(void) const;

    AudioManager* get_music_manager(void) const;

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

    void add_sfx_manager(AudioManager* extra_sfx_manager);

    void create_base_audio_managers(void);

    /**
     * enable_music/enable_sound_effects are meant to be called in response
     * to a user request so sfxActive/musicActive represent how things
     * *should* be, regardless of App/OS/HW state.
     */
    void enable_music(bool enable);

    void set_all_sfx_enables(bool enable);

    void enable_sound_effects(bool enable_sound_effects);

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

    void play_sfx(AudioSound* sfx, bool looping=false, bool interrupt=true, boost::optional<float> volume={},
        float time=0.0, boost::optional<NodePath> node={}, boost::optional<NodePath> listener_node={},
        boost::optional<float> cutoff={});

    void play_music(AudioSound* music, bool looping=false, bool interrupt=true, float time=0.0f, boost::optional<float> volume={});

    void run(void);

protected:
    GraphicsEngine* graphics_engine_ = nullptr;
    GraphicsWindow* win = nullptr;

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

    NodePath mouse_watcher;
    MouseWatcher* mouse_watcher_node;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ************************************************************************************************
inline GraphicsEngine* ShowBase::get_graphics_engine(void) const
{
    return graphics_engine_;
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

inline MouseWatcher* ShowBase::get_mouse_watcher_node(void) const
{
    return mouse_watcher_node;
}

}
