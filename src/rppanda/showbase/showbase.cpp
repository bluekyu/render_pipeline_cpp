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
 * This is C++ porting codes of direct/src/showbase/ShowBase.py
 */

#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include <cIntervalManager.h>
#include <pandaSystem.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <depthTestAttrib.h>
#include <depthWriteAttrib.h>
#include <pgTop.h>
#include <orthographicLens.h>
#include <audioManager.h>

#include "render_pipeline/rppanda/showbase/sfx_player.hpp"
#include "render_pipeline/rppanda/showbase/loader.hpp"
#include "render_pipeline/rppanda/showbase/messenger.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"

#include "rppanda/showbase/config_rppanda_showbase.hpp"

namespace rppanda {

class ShowBase::Impl
{
public:
    AsyncTask::DoneStatus ival_loop();
    AsyncTask::DoneStatus audio_loop();

    void initailize(ShowBase* self);

    void setup_mouse(ShowBase* self);
    void setup_render_2dp(ShowBase* self);
    void setup_render_2d(ShowBase* self);

    void add_sfx_manager(AudioManager* extra_sfx_manager);
    void create_base_audio_managers();
    void enable_music(bool enable);

    Filename get_screenshot_filename(const std::string& name_prefix, bool default_filename) const;
    void send_screenshot_event(const Filename& filename) const;

    void window_event(ShowBase* self, const Event* ev);

public:
    static ShowBase* global_ptr;

    std::shared_ptr<PandaFramework> panda_framework_;
    WindowFramework* window_framework_ = nullptr;

    std::unique_ptr<rppanda::Loader> loader_;
    Messenger* messenger_ = nullptr;
    TaskManager* task_mgr_ = nullptr;
    GraphicsEngine* graphics_engine_ = nullptr;
    GraphicsWindow* win_ = nullptr;

    PT(SfxPlayer) sfx_player_;
    PT(AudioManager) sfx_manager_;
    PT(AudioManager) music_manager_;

    std::vector<PT(AudioManager)> sfx_manager_list_;
    std::vector<bool> sfx_manager_is_valid_list_;

    bool want_render_2dp_;
    float config_aspect_ratio_;
    std::string window_type_;
    bool require_window_;

    NodePath hidden_;

    NodePath render_2dp_;
    NodePath aspect_2dp_;
    NodePath pixel_2dp_;

    NodePath camera2dp_;
    NodePath cam2dp_;

    float a2dp_top_;
    float a2dp_bottom_;
    float a2dp_left_;
    float a2dp_right_;

    NodePath a2dp_top_center_;
    NodePath a2dp_bottom_center_;
    NodePath a2dp_left_center_;
    NodePath a2dp_right_center_;

    NodePath a2dp_top_left_;
    NodePath a2dp_top_right_;
    NodePath a2dp_bottom_left_;
    NodePath a2dp_bottom_right_;

    NodePath a2d_background_;
    float a2d_top_;
    float a2d_bottom_;
    float a2d_left_;
    float a2d_right_;

    NodePath a2d_top_center_;
    NodePath a2d_top_center_ns_;
    NodePath a2d_bottom_center_;
    NodePath a2d_bottom_center_ns_;
    NodePath a2d_left_center_;
    NodePath a2d_left_center_ns_;
    NodePath a2d_right_center_;
    NodePath a2d_right_center_ns_;

    NodePath a2d_top_left_;
    NodePath a2d_top_left_ns_;
    NodePath a2d_top_right_;
    NodePath a2d_top_right_ns_;
    NodePath a2d_bottom_left_;
    NodePath a2d_bottom_left_ns_;
    NodePath a2d_bottom_right_;
    NodePath a2d_bottom_right_ns_;

    NodePath mouse_watcher_;
    MouseWatcher* mouse_watcher_node_;

    bool app_has_audio_focus_ = true;
    bool music_manager_is_valid_ = false;
    bool sfx_active_ = false;
    bool music_active_ = false;

    bool backface_culling_enabled_;
    bool wireframe_enabled_;

    WindowProperties prev_window_properties_;

    std::function<void()> exit_func_;
    std::vector<std::function<void()>> final_exit_callbacks_;
};

ShowBase* ShowBase::Impl::global_ptr = nullptr;

AsyncTask::DoneStatus ShowBase::Impl::ival_loop()
{
    // Execute all intervals in the global ivalMgr.
    CIntervalManager::get_global_ptr()->step();
    return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus ShowBase::Impl::audio_loop()
{
    if (music_manager_)
    {
        music_manager_->update();
    }

    for (auto&& x: sfx_manager_list_)
        x->update();

    return AsyncTask::DS_cont;
}

void ShowBase::Impl::initailize(ShowBase* self)
{
    if (Impl::global_ptr)
    {
        rppanda_showbase_cat.error() << "ShowBase was already created!" << std::endl;
        return;
    }

    Impl::global_ptr = self;

    rppanda_showbase_cat.debug() << "Creating ShowBase ..." << std::endl;

    if (panda_framework_->get_num_windows() == 0)
        window_framework_ = panda_framework_->open_window();
    else
        window_framework_ = panda_framework_->get_window(0);

    sfx_active_ = ConfigVariableBool("audio-sfx-active", true).get_value();
    music_active_ = ConfigVariableBool("audio-music-active", true).get_value();
    want_render_2dp_ = ConfigVariableBool("want-render2dp", true).get_value();

    // Fill this in with a function to invoke when the user "exits"
    // the program by closing the main window.
    exit_func_ = std::function<void()>();

    // Add final-exit callbacks to this list.  These will be called
    // when sys.exit() is called, after Panda has unloaded, and
    // just before Python is about to shut down.
    final_exit_callbacks_.clear();

    // screenshot_extension in config_display.h

    // If the aspect ratio is 0 or None, it means to infer the
    // aspect ratio from the window size.
    // If you need to know the actual aspect ratio call base.getAspectRatio()

    // aspect_ratio in config_framework.h, but NOT exported.
    config_aspect_ratio_ = ConfigVariableDouble("aspect-ratio", 0).get_value();

    // This is set to the value of the window-type config variable, but may
    // optionally be overridden in the Showbase constructor.  Should either be
    // 'onscreen' (the default), 'offscreen' or 'none'.

    // window_type in config_framework.h, but NOT exported.
    window_type_ = ConfigVariableString("window-type", "onscreen").get_value();
    require_window_ = ConfigVariableBool("require-window", true).get_value();

    hidden_ = NodePath("hidden");

    // The global graphics engine, ie. GraphicsEngine.getGlobalPtr()
    graphics_engine_ = GraphicsEngine::get_global_ptr();
    self->setup_render();
    self->setup_render_2d();
    self->setup_data_graph();

    if (want_render_2dp_)
        self->setup_render_2dp();

    win_ = window_framework_->get_graphics_window();

    // Open the default rendering window.
    self->open_default_window();

    // The default is trackball mode, which is more convenient for
    // ad-hoc development in Python using ShowBase.Applications
    // can explicitly call base.useDrive() if they prefer a drive
    // interface.
    self->use_trackball();

    loader_ = std::make_unique<rppanda::Loader>(*self);

    messenger_ = Messenger::get_global_instance();
    task_mgr_ = TaskManager::get_global_instance();

    app_has_audio_focus_ = true;

    self->create_base_audio_managers();

    // Now hang a hook on the window-event from Panda. This allows
    // us to detect when the user resizes, minimizes, or closes the
    // main window.
    prev_window_properties_.clear();
    self->accept("window-event", [self](const Event* ev) {
        self->window_event(ev);
    });

    self->restart();
}

void ShowBase::Impl::setup_mouse(ShowBase* self)
{
    self->setup_mouse_cb();

    mouse_watcher_ = window_framework_->get_mouse();
    mouse_watcher_node_ = DCAST(MouseWatcher, mouse_watcher_.node());

    // In C++, aspect2d has already mouse watcher.
    DCAST(PGTop, self->get_pixel_2d().node())->set_mouse_watcher(mouse_watcher_node_);

    if (want_render_2dp_)
    {
        DCAST(PGTop, aspect_2dp_.node())->set_mouse_watcher(mouse_watcher_node_);
        DCAST(PGTop, pixel_2dp_.node())->set_mouse_watcher(mouse_watcher_node_);
    }
}

void ShowBase::Impl::setup_render_2dp(ShowBase* self)
{
    rppanda_showbase_cat.debug() << "Setup 2D nodes." << std::endl;

    render_2dp_ = NodePath("render2dp");

    // Set up some overrides to turn off certain properties which
    // we probably won't need for 2-d objects.

    // It's probably important to turn off the depth test, since
    // many 2-d objects will be drawn over each other without
    // regard to depth position.

    render_2dp_.set_depth_test(0);
    render_2dp_.set_depth_write(0);

    render_2dp_.set_material_off(1);
    render_2dp_.set_two_sided(1);

    // The normal 2-d DisplayRegion has an aspect ratio that
    // matches the window, but its coordinate system is square.
    // This means anything we parent to render2dp gets stretched.
    // For things where that makes a difference, we set up
    // aspect2dp, which scales things back to the right aspect
    // ratio along the X axis (Z is still from -1 to 1)
    PT(PGTop) aspect_2dp_pg_top = new PGTop("aspect2dp");
    aspect_2dp_ = render_2dp_.attach_new_node(aspect_2dp_pg_top);
    aspect_2dp_pg_top->set_start_sort(16384);

    const float aspect_ratio = self->get_aspect_ratio();
    aspect_2dp_.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);

    // The Z position of the top border of the aspect2dp screen.
    a2dp_top_ = 1.0f;
    // The Z position of the bottom border of the aspect2dp screen.
    a2dp_bottom_ = -1.0f;
    // The X position of the left border of the aspect2dp screen.
    a2dp_left_ = -aspect_ratio;
    // The X position of the right border of the aspect2dp screen.
    a2dp_right_ = aspect_ratio;

    a2dp_top_center_ = aspect_2dp_.attach_new_node("a2dpTopCenter");
    a2dp_bottom_center_ = aspect_2dp_.attach_new_node("a2dpBottomCenter");
    a2dp_left_center_ = aspect_2dp_.attach_new_node("a2dpLeftCenter");
    a2dp_right_center_ = aspect_2dp_.attach_new_node("a2dpRightCenter");

    a2dp_top_left_ = aspect_2dp_.attach_new_node("a2dpTopLeft");
    a2dp_top_right_ = aspect_2dp_.attach_new_node("a2dpTopRight");
    a2dp_bottom_left_ = aspect_2dp_.attach_new_node("a2dpBottomLeft");
    a2dp_bottom_right_ = aspect_2dp_.attach_new_node("a2dpBottomRight");

    // Put the nodes in their places
    a2dp_top_center_.set_pos(0, 0, a2dp_top_);
    a2dp_bottom_center_.set_pos(0, 0, a2dp_bottom_);
    a2dp_left_center_.set_pos(a2dp_left_, 0, 0);
    a2dp_right_center_.set_pos(a2dp_right_, 0, 0);

    a2dp_top_left_.set_pos(a2dp_left_, 0, a2dp_top_);
    a2dp_top_right_.set_pos(a2dp_right_, 0, a2dp_top_);
    a2dp_bottom_left_.set_pos(a2dp_left_, 0, a2dp_bottom_);
    a2dp_bottom_right_.set_pos(a2dp_right_, 0, a2dp_bottom_);

    // This special root, pixel2d, uses units in pixels that are relative
    // to the window. The upperleft corner of the window is (0, 0),
    // the lowerleft corner is (xsize, -ysize), in this coordinate system.
    PT(PGTop) pixel_2dp_pg_top = new PGTop("pixel2dp");
    pixel_2dp_ = render_2dp_.attach_new_node(pixel_2dp_pg_top);
    pixel_2dp_pg_top->set_start_sort(16384);
    pixel_2dp_.set_pos(-1, 0, 1);
    const LVecBase2i& size = self->get_size();
    float xsize = size.get_x();
    float ysize = size.get_y();
    if (xsize > 0 && ysize > 0)
        pixel_2dp_.set_scale(2.0f / xsize, 1.0f, 2.0f / ysize);
}

void ShowBase::Impl::setup_render_2d(ShowBase* self)
{
    // Window framework already created render2d.
    NodePath render2d = self->get_render_2d();

    // Window framework already created aspect2d.
    NodePath aspect_2d = self->get_aspect_2d();

    a2d_background_ = aspect_2d.attach_new_node("a2d_background");

    const float aspect_ratio = self->get_aspect_ratio();

    // The Z position of the top border of the aspect2d screen.
    a2d_top_ = 1.0f;
    // The Z position of the bottom border of the aspect2d screen.
    a2d_bottom_ = -1.0f;
    // The X position of the left border of the aspect2d screen.
    a2d_left_ = -aspect_ratio;
    // The X position of the right border of the aspect2d screen.
    a2d_right_ = aspect_ratio;

    a2d_top_center_ = aspect_2d.attach_new_node("a2d_top_center");
    a2d_top_center_ns_ = aspect_2d.attach_new_node("a2d_top_center_ns");
    a2d_bottom_center_ = aspect_2d.attach_new_node("a2d_bottom_center");
    a2d_bottom_center_ns_ = aspect_2d.attach_new_node("a2d_bottom_center_ns");
    a2d_left_center_ = aspect_2d.attach_new_node("a2d_left_center");
    a2d_left_center_ns_ = aspect_2d.attach_new_node("a2d_left_center_ns");
    a2d_right_center_ = aspect_2d.attach_new_node("a2d_right_center");
    a2d_right_center_ns_ = aspect_2d.attach_new_node("a2d_right_center_ns");

    a2d_top_left_ = aspect_2d.attach_new_node("a2d_top_left");
    a2d_top_left_ns_ = aspect_2d.attach_new_node("a2d_top_left_ns");
    a2d_top_right_ = aspect_2d.attach_new_node("a2d_top_right");
    a2d_top_right_ns_ = aspect_2d.attach_new_node("a2d_top_right_ns");
    a2d_bottom_left_ = aspect_2d.attach_new_node("a2d_bottom_left");
    a2d_bottom_left_ns_ = aspect_2d.attach_new_node("a2d_bottom_left_ns");
    a2d_bottom_right_ = aspect_2d.attach_new_node("a2d_bottom_right");
    a2d_bottom_right_ns_ = aspect_2d.attach_new_node("a2d_bottom_right_ns");

    // Put the nodes in their places
    a2d_top_center_.set_pos(0, 0, a2d_top_);
    a2d_top_center_ns_.set_pos(0, 0, a2d_top_);
    a2d_bottom_center_.set_pos(0, 0, a2d_bottom_);
    a2d_bottom_center_ns_.set_pos(0, 0, a2d_bottom_);
    a2d_left_center_.set_pos(a2d_left_, 0, 0);
    a2d_left_center_ns_.set_pos(a2d_left_, 0, 0);
    a2d_right_center_.set_pos(a2d_right_, 0, 0);
    a2d_right_center_ns_.set_pos(a2d_right_, 0, 0);

    a2d_top_left_.set_pos(a2d_left_, 0, a2d_top_);
    a2d_top_left_ns_.set_pos(a2d_left_, 0, a2d_top_);
    a2d_top_right_.set_pos(a2d_right_, 0, a2d_top_);
    a2d_top_right_ns_.set_pos(a2d_right_, 0, a2d_top_);
    a2d_bottom_left_.set_pos(a2d_left_, 0, a2d_bottom_);
    a2d_bottom_left_ns_.set_pos(a2d_left_, 0, a2d_bottom_);
    a2d_bottom_right_.set_pos(a2d_right_, 0, a2d_bottom_);
    a2d_bottom_right_ns_.set_pos(a2d_right_, 0, a2d_bottom_);

    // pixel2d is created in window framework.
}

void ShowBase::Impl::add_sfx_manager(AudioManager* extra_sfx_manager)
{
    // keep a list of sfx manager objects to apply settings to,
    // since there may be others in addition to the one we create here
    sfx_manager_list_.push_back(extra_sfx_manager);
    bool new_sfx_manager_is_valid = extra_sfx_manager && extra_sfx_manager->is_valid();
    sfx_manager_is_valid_list_.push_back(new_sfx_manager_is_valid);
    if (new_sfx_manager_is_valid)
        extra_sfx_manager->set_active(sfx_active_);
}

void ShowBase::Impl::create_base_audio_managers()
{
    rppanda_showbase_cat.debug() << "Creating base audio manager ..." << std::endl;

    sfx_player_ = new SfxPlayer;
    sfx_manager_ = AudioManager::create_AudioManager();
    add_sfx_manager(sfx_manager_);

    music_manager_ = AudioManager::create_AudioManager();
    music_manager_is_valid_ = music_manager_ && music_manager_->is_valid();
    if (music_manager_is_valid_)
    {
        // ensure only 1 midi song is playing at a time:
        music_manager_->set_concurrent_sound_limit(1);
        music_manager_->set_active(music_active_);
    }
}

void ShowBase::Impl::enable_music(bool enable)
{
    // don't setActive(1) if no audiofocus
    if (app_has_audio_focus_ && music_manager_is_valid_)
        music_manager_->set_active(enable);
    music_active_ = enable;
    if (enable)
    {
        // This is useful when we want to play different music
        // from what the manager has queued
        messenger_->send("MusicEnabled");
        rppanda_showbase_cat.debug() << "Enabling music" << std::endl;
    }
    else
    {
        rppanda_showbase_cat.debug() << "Disabling music" << std::endl;
    }
}

Filename ShowBase::Impl::get_screenshot_filename(const std::string& name_prefix, bool default_filename) const
{
    if (default_filename)
        return GraphicsOutput::make_screenshot_filename(name_prefix);
    else
        return Filename(name_prefix);
}

void ShowBase::Impl::send_screenshot_event(const Filename& filename) const
{
    // Announce to anybody that a screenshot has been taken
    messenger_->send("screenshot", EventParameter(filename));
}

void ShowBase::Impl::window_event(ShowBase* self, const Event* ev)
{
    // See PandaFramework::event_window_event
    if (ev->get_num_parameters() != 1)
        return;

    EventParameter param = ev->get_parameter(0);
    const GraphicsWindow* win;
    DCAST_INTO_V(win, param.get_ptr());

    if (win != self->get_win())
    {
        // This event isn't about our window.
        return;
    }

    auto properties = win->get_properties();
    if (properties == prev_window_properties_)
        return;

    prev_window_properties_ = properties;

    rppanda_showbase_cat.debug() << "Got Window event: " << properties << std::endl;
    if (!properties.get_open())
    {
        // If the user closes the main window, we should exit.
        rppanda_showbase_cat.info("User closed main window.");
        self->user_exit();
    }
}

// ************************************************************************************************

TypeHandle ShowBase::type_handle_;

ShowBase* ShowBase::get_global_ptr()
{
    return Impl::global_ptr;
}

ShowBase::ShowBase() : impl_(std::make_unique<Impl>())
{
}

ShowBase::ShowBase(int argc, char* argv[]): ShowBase()
{
    initialize(argc, argv);
}

ShowBase::ShowBase(PandaFramework* framework): ShowBase()
{
    initialize(framework);
}

ShowBase::~ShowBase()
{
    destroy();
}

void ShowBase::setup_render_2d() { impl_->setup_render_2d(this); }
void ShowBase::setup_render_2dp() { impl_->setup_render_2dp(this); }
void ShowBase::setup_mouse() { impl_->setup_mouse(this); }
void ShowBase::create_base_audio_managers() { impl_->create_base_audio_managers(); }
void ShowBase::add_sfx_manager(AudioManager* extra_sfx_manager) { impl_->add_sfx_manager(extra_sfx_manager); }
void ShowBase::enable_music(bool enable) { impl_->enable_music(enable); }

void ShowBase::initialize(int argc, char* argv[])
{
    impl_->panda_framework_ = std::make_shared<PandaFramework>();
    impl_->panda_framework_->open_framework(argc, argv);

    impl_->initailize(this);
}

void ShowBase::initialize(PandaFramework* framework)
{
    impl_->panda_framework_ = std::shared_ptr<PandaFramework>(framework, [](auto) {});

    impl_->initailize(this);
}

void ShowBase::destroy()
{
    if (!impl_)
        return;

    for (const auto& cb : impl_->final_exit_callbacks_)
        cb();

    get_aspect_2d().node()->remove_all_children();

    ignore_all();
    shutdown();

    if (impl_->music_manager_)
    {
        impl_->music_manager_->shutdown();
        impl_->music_manager_.clear();
        for (auto&& manager: impl_->sfx_manager_list_)
            manager->shutdown();
        impl_->sfx_manager_list_.clear();
        impl_->sfx_manager_is_valid_list_.clear();
    }

    impl_->loader_.reset();

    // will remove in PandaFramework::close_framework() or PandaFramework::~PandaFramework()
    //impl_->graphics_engine_->remove_all_windows();

    Impl::global_ptr = nullptr;

    // clear all data in ShowBase::Impl except PandaFramework
    auto hold = impl_->panda_framework_;
    impl_.reset();

    // PandaFramework::~PandaFramework() calls PandaFramework::close_framework()
}

PandaFramework* ShowBase::get_panda_framework() const
{
    return impl_->panda_framework_.get();
}

WindowFramework* ShowBase::get_window_framework() const
{
    return impl_->window_framework_;
}

rppanda::Loader* ShowBase::get_loader() const
{
    return impl_->loader_.get();
}

Messenger* ShowBase::get_messenger() const
{
    return impl_->messenger_;
}

TaskManager* ShowBase::get_task_mgr() const
{
    return impl_->task_mgr_;
}

GraphicsEngine* ShowBase::get_graphics_engine() const
{
    return impl_->graphics_engine_;
}

GraphicsWindow* ShowBase::get_win() const
{
    return impl_->win_;
}

const std::vector<PT(AudioManager)>& ShowBase::get_sfx_manager_list() const
{
    return impl_->sfx_manager_list_;
}

SfxPlayer* ShowBase::get_sfx_player() const
{
    return impl_->sfx_player_;
}

AudioManager* ShowBase::get_music_manager() const
{
    return impl_->music_manager_;
}

NodePath ShowBase::get_hidden() const
{
    return impl_->hidden_;
}

NodePath ShowBase::get_render() const
{
    return impl_->window_framework_->get_render();
}

NodePath ShowBase::get_render_2d() const
{
    return impl_->window_framework_->get_render_2d();
}

NodePath ShowBase::get_aspect_2d() const
{
    return impl_->window_framework_->get_aspect_2d();
}

NodePath ShowBase::get_pixel_2d() const
{
    return impl_->window_framework_->get_pixel_2d();
}

NodePath ShowBase::get_render_2dp() const
{
    return impl_->render_2dp_;
}

NodePath ShowBase::get_pixel_2dp() const
{
    return impl_->pixel_2dp_;
}

float ShowBase::get_config_aspect_ratio() const
{
    return impl_->config_aspect_ratio_;
}

bool ShowBase::open_default_window()
{
    open_main_window();

    return impl_->win_ != nullptr;
}

void ShowBase::open_main_window()
{
    if (impl_->win_)
    {
        if (impl_->win_->is_of_type(GraphicsWindow::get_class_type()))
            setup_mouse();

        if (impl_->want_render_2dp_)
            make_camera2dp(impl_->win_);
    }
}

void ShowBase::setup_render()
{
    // C++ sets already render node.
    //self.render.setAttrib(RescaleNormalAttrib.makeDefault())
    //self.render.setTwoSided(0)

    NodePath render = get_render();
    impl_->backface_culling_enabled_ = render.get_two_sided();
    //textureEnabled = 1;
    impl_->wireframe_enabled_ = render.get_render_mode() == RenderModeAttrib::M_wireframe;
}

NodePath ShowBase::make_camera2dp(GraphicsWindow* win, int sort, const LVecBase4f& display_region,
    const LVecBase4f& coords, Lens* lens, const std::string& camera_name)
{
    rppanda_showbase_cat.debug() << "Making 2D camera ..." << std::endl;

    DisplayRegion* dr = win->make_mono_display_region(display_region);
    dr->set_sort(sort);

    // Unlike render2d, we don't clear the depth buffer for
    // render2dp.Caveat emptor.

    dr->set_incomplete_render(false);

    // Now make a new Camera node.
    PT(Camera) cam2d_node;
    if (camera_name.empty())
        cam2d_node = new Camera("cam2dp");
    else
        cam2d_node = new Camera(std::string("cam2dp_") + camera_name);

    const float left = coords.get_x();
    const float right = coords.get_y();
    const float bottom = coords.get_z();
    const float top = coords.get_w();

    if (!lens)
    {
        lens = new OrthographicLens;
        lens->set_film_size(right - left, top - bottom);
        lens->set_film_offset((right + left) * 0.5, (top + bottom) * 0.5);
        lens->set_near_far(-1000, 1000);
    }
    cam2d_node->set_lens(lens);

    // self.camera2d is the analog of self.camera, although it's
    // not as clear how useful it is.
    if (impl_->camera2dp_.is_empty())
        impl_->camera2dp_ = impl_->render_2dp_.attach_new_node("camera2dp");

    NodePath camera2dp = impl_->camera2dp_.attach_new_node(cam2d_node);
    dr->set_camera(camera2dp);

    if (impl_->cam2dp_.is_empty())
        impl_->cam2dp_ = camera2dp;

    return camera2dp;
}

void ShowBase::setup_data_graph()
{
}

void ShowBase::setup_mouse_cb()
{
    // Note that WindowFramework::get_mouse

    impl_->window_framework_->enable_keyboard();
}

void ShowBase::toggle_backface()
{
    if (impl_->backface_culling_enabled_)
        backface_culling_off();
    else
        backface_culling_on();
}

void ShowBase::backface_culling_on()
{
    if (!impl_->backface_culling_enabled_)
        get_render().set_two_sided(false);
    impl_->backface_culling_enabled_ = true;
}

void ShowBase::backface_culling_off()
{
    if (!impl_->backface_culling_enabled_)
        get_render().set_two_sided(true);
    impl_->backface_culling_enabled_ = false;
}

void ShowBase::toggle_wireframe()
{
    if (impl_->wireframe_enabled_)
        wireframe_off();
    else
        wireframe_on();
}

void ShowBase::wireframe_on()
{
    NodePath render = get_render();
    render.set_render_mode_wireframe(100);
    render.set_two_sided(1);
    impl_->wireframe_enabled_ = true;
}

void ShowBase::wireframe_off()
{
    NodePath render = get_render();
    render.clear_render_mode();
    render.set_two_sided(!impl_->backface_culling_enabled_);
    impl_->wireframe_enabled_ = false;
}

float ShowBase::get_aspect_ratio(GraphicsOutput* win) const
{
    // If the config it set, we return that
    if (impl_->config_aspect_ratio_)
        return impl_->config_aspect_ratio_;

    float aspect_ratio = 1.0f;

    if (!win)
        win = impl_->win_;

    if (win && win->has_size() && win->get_sbs_left_y_size() != 0)
    {
        aspect_ratio = static_cast<float>(win->get_sbs_left_x_size()) / win->get_sbs_left_y_size();
    }
    else
    {
        WindowProperties props;
        if (!win && !DCAST(GraphicsWindow, win))
        {
            props = WindowProperties::get_default();
        }
        else
        {
            props = DCAST(GraphicsWindow, win)->get_requested_properties();
            if (!props.has_size())
                props = WindowProperties::get_default();
        }

        if (props.has_size() && props.get_y_size() != 0)
            aspect_ratio = static_cast<float>(props.get_x_size()) / props.get_y_size();
    }

    return aspect_ratio == 0 ? 1 : aspect_ratio;
}

LVecBase2i ShowBase::get_size(GraphicsOutput* win) const
{
    if (!win)
        win = impl_->win_;

    if (win && win->has_size())
    {
        return win->get_size();
    }
    else
    {
        WindowProperties props;
        if (!win && !DCAST(GraphicsWindow, win))
        {
            props = WindowProperties::get_default();
        }
        else
        {
            props = DCAST(GraphicsWindow, win)->get_requested_properties();
            if (!props.has_size())
                props = WindowProperties::get_default();
        }

        return props.get_size();
    }
}

NodePath ShowBase::get_camera() const
{
    return impl_->window_framework_->get_camera_group();
}

NodePath ShowBase::get_cam(int index) const
{
    return NodePath(impl_->window_framework_->get_camera(index));
}

Camera* ShowBase::get_cam_node(int index) const
{
    return impl_->window_framework_->get_camera(index);
}

Lens* ShowBase::get_cam_lens(int cam_index, int lens_index) const
{
    return get_cam_node(cam_index)->get_lens(lens_index);
}

MouseWatcher* ShowBase::get_mouse_watcher_node() const
{
    return impl_->mouse_watcher_node_;
}

NodePath ShowBase::get_button_thrower() const
{
    // Node that WindowFramework::enable_keyboard
    return impl_->window_framework_->get_mouse().find("kb-events");
}

const NodePath& ShowBase::get_data_root() const
{
    return impl_->panda_framework_->get_data_root();
}

PandaNode* ShowBase::get_data_root_node() const
{
    return get_data_root().node();
}

void ShowBase::restart()
{
    rppanda_showbase_cat.debug() << "Re-staring ShowBase ..." << std::endl;

    shutdown();

    add_task(std::bind(&Impl::ival_loop, impl_.get()), "ival_loop", 20);

    // the audioLoop updates the positions of 3D sounds.
    // as such, it needs to run after the cull traversal in the igLoop.
    add_task(std::bind(&Impl::audio_loop, impl_.get()), "audio_loop", 60);
}

void ShowBase::shutdown()
{
    rppanda_showbase_cat.debug() << "Shutdown ShowBase ..." << std::endl;

    impl_->task_mgr_->remove("audio_loop");
    impl_->task_mgr_->remove("ival_loop");
}

void ShowBase::disable_mouse()
{
    // Note that WindowFramework::setup_trackball.
    NodePath tball2cam = impl_->window_framework_->get_mouse().find("trackball/tball2cam");
    if (!tball2cam.is_empty())
        tball2cam.detach_node();
}

void ShowBase::use_trackball()
{
    impl_->window_framework_->setup_trackball();
}

void ShowBase::set_all_sfx_enables(bool enable)
{
    for (size_t k=0, k_end=impl_->sfx_manager_list_.size(); k < k_end; ++k)
    {
        if (impl_->sfx_manager_is_valid_list_[k])
            impl_->sfx_manager_list_[k]->set_active(enable);
    }
}

void ShowBase::disable_all_audio()
{
    impl_->app_has_audio_focus_ = false;
    set_all_sfx_enables(false);
    if (impl_->music_manager_is_valid_)
        impl_->music_manager_->set_active(false);
    rppanda_showbase_cat.debug() << "Disabling audio" << std::endl;
}

void ShowBase::enable_all_audio()
{
    impl_->app_has_audio_focus_ = true;
    set_all_sfx_enables(impl_->sfx_active_);
    if (impl_->music_manager_is_valid_)
        impl_->music_manager_->set_active(impl_->music_active_);
    rppanda_showbase_cat.debug() << "Enabling audio" << std::endl;
}

void ShowBase::enable_sound_effects(bool enable_sound_effects)
{
    // don't setActive(1) if no audiofocus
    if (impl_->app_has_audio_focus_ || !enable_sound_effects)
        set_all_sfx_enables(enable_sound_effects);
    impl_->sfx_active_ = enable_sound_effects;
    if (enable_sound_effects)
        rppanda_showbase_cat.debug() << "Enabling sound effects" << std::endl;
    else
        rppanda_showbase_cat.debug() << "Disabling sound effects" << std::endl;
}

void ShowBase::play_sfx(AudioSound* sfx, bool looping, bool interrupt, boost::optional<float> volume,
    float time, boost::optional<NodePath> node, boost::optional<NodePath> listener_node,
    boost::optional<float> cutoff)
{
    impl_->sfx_player_->play_sfx(sfx, looping, interrupt, volume, time, node, listener_node, cutoff);
}

void ShowBase::play_music(AudioSound* music, bool looping, bool interrupt, float time, boost::optional<float> volume)
{
    if (!music)
        return;

    if (volume)
        music->set_volume(volume.value());

    if (interrupt || (music->status() != AudioSound::PLAYING))
    {
        music->set_time(time);
        music->set_loop(looping);
        music->play();
    }
}

Filename ShowBase::screenshot(const std::string& name_prefix, bool default_filename,
    const std::string& image_comment)
{
    Filename filename = impl_->get_screenshot_filename(name_prefix, default_filename);

    bool saved = impl_->win_->save_screenshot(filename, image_comment);
    if (saved)
    {
        impl_->send_screenshot_event(filename);
        return filename;
    }

    return "";
}

Filename ShowBase::screenshot(GraphicsOutput* source, const std::string& name_prefix, bool default_filename,
    const std::string& image_comment)
{
    if (!source)
    {
        rppanda_showbase_cat.error() << "ShowBase::screenshot source is nullptr." << std::endl;
        return "";
    }

    Filename filename = impl_->get_screenshot_filename(name_prefix, default_filename);

    bool saved = source->save_screenshot(filename, image_comment);
    if (saved)
    {
        impl_->send_screenshot_event(filename);
        return filename;
    }

    return "";
}

Filename ShowBase::screenshot(Texture* source, const std::string& name_prefix, bool default_filename,
    const std::string& image_comment)
{
    if (!source)
    {
        rppanda_showbase_cat.error() << "ShowBase::screenshot source is nullptr." << std::endl;
        return "";
    }

    Filename filename = impl_->get_screenshot_filename(name_prefix, default_filename);

    bool saved = false;
    if (source->get_z_size() > 1)
        saved = source->write(filename, 0, 0, true, false);
    else
        saved = source->write(filename);

    if (saved)
    {
        impl_->send_screenshot_event(filename);
        return filename;
    }

    return "";
}

Filename ShowBase::screenshot(DisplayRegion* source, const std::string& name_prefix, bool default_filename,
    const std::string& image_comment)
{
    if (!source)
    {
        rppanda_showbase_cat.error() << "ShowBase::screenshot source is nullptr." << std::endl;
        return "";
    }

    Filename filename = impl_->get_screenshot_filename(name_prefix, default_filename);

    bool saved = source->save_screenshot(filename, image_comment);
    if (saved)
    {
        impl_->send_screenshot_event(filename);
        return filename;
    }

    return "";
}

void ShowBase::window_event(const Event* ev)
{
    impl_->window_event(this, ev);
}

void ShowBase::user_exit()
{
    // The user has requested we exit the program.Deal with this.
    if (impl_->exit_func_)
        impl_->exit_func_();
    rppanda_showbase_cat.info() << "Exiting ShowBase." << std::endl;
    finalize_exit();
}

void ShowBase::finalize_exit()
{
    impl_->panda_framework_->set_exit_flag();
}

void ShowBase::run()
{
    impl_->panda_framework_->main_loop();
}

void ShowBase::set_exit_func(const std::function<void()>& exit_func)
{
    impl_->exit_func_ = exit_func;
}

std::vector<std::function<void()>>& ShowBase::get_final_exit_callbacks()
{
    return impl_->final_exit_callbacks_;
}

const std::vector<std::function<void()>>& ShowBase::get_final_exit_callbacks() const
{
    return impl_->final_exit_callbacks_;
}

}
