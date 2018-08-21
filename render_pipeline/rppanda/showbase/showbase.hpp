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
class AudioManager;
class AudioSound;

namespace rppanda {

class SfxPlayer;
class Loader;
class TaskManager;
class Messenger;

class RENDER_PIPELINE_DECL ShowBase : public DirectObject
{
public:
    static ShowBase* get_global_ptr();

    /**
     * Create ShowBase and initialize ShowBase if @p lazy_initialize is false.
     *
     * Without @p lazy_initialize, this will create PandaFramework and open main window,
     * and then initialize ShowBase.
     *
     * With @p lazy_initialize, this just consturct ShowBase instance,
     * so you should call ShowBase::initialize.
     */
    ShowBase(bool lazy_initialize = false);

    /**
     * Initialize ShowBase with given PandaFramework.
     */
    ShowBase(PandaFramework* framework);

    ShowBase(const ShowBase&) = delete;
    ShowBase(ShowBase&&);

    virtual ~ShowBase();

    ShowBase& operator=(const ShowBase&) = delete;
    ShowBase& operator=(ShowBase&&);

    void initialize(PandaFramework* framework = nullptr);

    /**
     * Call this function to destroy the ShowBase and stop all
     * its tasks, freeing all of the Panda resources.  Normally, you
     * should not need to call it explicitly, as it is bound to the
     * exitfunc and will be called at application exit time
     * automatically.
     *
     * This function is designed to be safe to call multiple times.
     */
    void destroy();

    PandaFramework* get_panda_framework() const;
    WindowFramework* get_window_framework() const;

    rppanda::Loader* get_loader() const;
    Messenger* get_messenger() const;
    TaskManager* get_task_mgr() const;
    GraphicsEngine* get_graphics_engine() const;
    GraphicsWindow* get_win() const;

    SfxPlayer* get_sfx_player() const;
    const std::vector<PT(AudioManager)>& get_sfx_manager_list() const;
    AudioManager* get_music_manager() const;

    NodePath get_hidden() const;
    NodePath get_render() const;
    NodePath get_render_2d() const;
    NodePath get_aspect_2d() const;
    NodePath get_pixel_2d() const;
    NodePath get_render_2dp() const;
    NodePath get_pixel_2dp() const;
    float get_config_aspect_ratio() const;

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
    LVecBase2i get_size(GraphicsOutput* win=nullptr) const;

    /**
     * This is the NodePath that should be used to manipulate the camera.
     * This is the node to which the default camera is attached.
     */
     ///@{

     /**
      * self.camera is the parent node of all cameras: a node that
      * we can move around to move all cameras as a group.
      */
    NodePath get_camera() const;
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

    MouseWatcher* get_mouse_watcher_node() const;
    NodePath get_button_thrower() const;

    const NodePath& get_data_root() const;
    PandaNode* get_data_root_node() const;

    bool open_default_window();

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
    void open_main_window();

    /**
     * Creates a render2d scene graph, the secondary scene graph for
     * 2-d objects and gui elements that are superimposed over the
     * 2-d and 3-d geometry in the window.
     */
    void setup_render_2dp();

    void setup_render();

    void setup_render_2d();

    NodePath make_camera2dp(GraphicsWindow* win, int sort=20,
        const LVecBase4f& display_region=LVecBase4f(0, 1, 0, 1),
        const LVecBase4f& coords=LVecBase4f(-1, 1, -1, 1),
        Lens* lens=nullptr, const std::string& camera_name="");

    void setup_data_graph();

    void setup_mouse();
    void setup_mouse_cb();

    void toggle_backface();
    void backface_culling_on();
    void backface_culling_off();

    void toggle_wireframe();
    void wireframe_on();
    void wireframe_off();

    void restart();

    void shutdown();

    void disable_mouse();

    void use_trackball();

    void add_sfx_manager(AudioManager* extra_sfx_manager);

    void create_base_audio_managers();

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
    void disable_all_audio();

    /** @see disable_all_audio() */
    void enable_all_audio();

    void play_sfx(AudioSound* sfx, bool looping=false, bool interrupt=true, boost::optional<float> volume=boost::none,
        float time=0.0, boost::optional<NodePath> node=boost::none, boost::optional<NodePath> listener_node=boost::none,
        boost::optional<float> cutoff=boost::none);

    void play_music(AudioSound* music, bool looping=false, bool interrupt=true, float time=0.0f, boost::optional<float> volume=boost::none);

    /**
     * Captures a screenshot from the main window and writes it to a filename
     * in the current directory (or to a specified directory).
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
    Filename screenshot(const std::string& name_prefix = "screenshot", bool default_filename = true,
        const std::string& image_comment = "");

    /**
     * Captures a screenshot from the specified window and writes it to a filename
     * in the current directory (or to a specified directory).
     *
     * @override screenshot(const std::string&, bool, const std::string&)
     */
    Filename screenshot(GraphicsOutput* source, const std::string& name_prefix="screenshot", bool default_filename=true,
        const std::string& image_comment="");

    /**
     * Captures a screenshot from Texture and writes it to a filename in the
     * current directory (or to a specified directory).
     *
     * @override screenshot(const std::string&, bool, const std::string&)
     */
    Filename screenshot(Texture* source, const std::string& name_prefix="screenshot", bool default_filename=true,
        const std::string& image_comment="");

    /**
     * Captures a screenshot from DispayRegion and writes it to a filename in the
     * current directory (or to a specified directory).
     *
     * @override screenshot(const std::string&, bool, const std::string&)
     */
    Filename screenshot(DisplayRegion* source, const std::string& name_prefix="screenshot", bool default_filename=true,
        const std::string& image_comment="");

    void window_event(const Event* ev);

    void user_exit();
    void finalize_exit();

    void run();

    void set_exit_func(const std::function<void()>& exit_func);

    /**
     * Add final-exit callbacks to this list. These will be called
     * when ShowBase is removed, after Panda has unloaded.
     */
    std::vector<std::function<void()>>& get_final_exit_callbacks();
    const std::vector<std::function<void()>>& get_final_exit_callbacks() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
