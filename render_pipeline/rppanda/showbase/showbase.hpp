/**
 * This module contains ShowBase, an application framework responsible
 * for opening a graphical display, setting up input devices and creating
 * the scene graph.
 *
 * This is C++ porting codes of direct/src/showbase/ShowBase.py
 */

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

class SfxPlayer;

class RENDER_PIPELINE_DECL ShowBase: public DirectObject
{
public:
    /** Create PandaFramework and open main window, and then initialize ShowBase. */
    ShowBase(int& argc, char**& argv);

    /** Initialize ShowBase with given PandaFramework. */
    ShowBase(PandaFramework* framework);

    ~ShowBase(void);

    ShowBase(const ShowBase&) = delete;
    ShowBase(ShowBase&&) = delete;
    ShowBase& operator=(const ShowBase&) = delete;
    ShowBase& operator=(ShowBase&&) = delete;

    static ShowBase* get_global_ptr(void);

    PandaFramework* get_panda_framework(void) const;
    WindowFramework* get_window_framework(void) const;

    GraphicsEngine* get_graphics_engine(void) const;
    GraphicsWindow* get_win(void) const;

    SfxPlayer* get_sfx_player(void) const;
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

    void setup_render_2d(void);

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

    /**
     * Captures a screenshot from the main window or from the
     * specified window and writes it to a filename in the
     * current directory (or to a specified directory).
     *
     * If @p default_filename is true, the filename is synthesized by
     * appending @p name_prefix to a default filename suffix (including
     * the filename extension) specified in the Config variable
     * screenshot-filename.  Otherwise, if @p default_filename is False,
     * the entire @p name_prefix is taken to be the filename to write,
     * and this string should include a suitable filename extension
     * that will be used to determine the type of image file to
     * write.
     *
     * @return value is the filename if successful, or empty if there is a problem.
     */
    Filename screenshot(GraphicsOutput* source=nullptr, const std::string& name_prefix="screenshot", bool default_filename=true,
        const std::string& image_comment="");

    /**
     * Captures a screenshot from Texture and writes it to a filename in the
     * current directory (or to a specified directory).
     *
     * @override screenshot(GraphicsOutput*, const std::string&, bool, const std::string&)
     */
    Filename screenshot(Texture* source, const std::string& name_prefix="screenshot", bool default_filename=true, 
        const std::string& image_comment="");

    /**
     * Captures a screenshot from DispayRegion and writes it to a filename in the
     * current directory (or to a specified directory).
     *
     * @override screenshot(GraphicsOutput*, const std::string&, bool, const std::string&)
     */
    Filename screenshot(DisplayRegion* source, const std::string& name_prefix="screenshot", bool default_filename=true,
        const std::string& image_comment="");

    void run(void);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
