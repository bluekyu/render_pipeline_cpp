#include "render_pipeline/rppanda/showbase/showbase.hpp"

#include <cIntervalManager.h>
#include <pandaSystem.h>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <depthTestAttrib.h>
#include <depthWriteAttrib.h>
#include <pgTop.h>
#include <asyncTaskManager.h>
#include <pgMouseWatcherBackground.h>
#include <orthographicLens.h>
#include <audioManager.h>
#include <throw_event.h>

#include "rppanda/config_rppanda.h"

namespace rppanda {

static ShowBase* global_showbase = nullptr;

struct ShowBase::Impl
{
    static AsyncTask::DoneStatus audio_loop(GenericAsyncTask *task, void *user_data);

    void create_base_audio_managers(void);
    void enable_music(bool enable);

public:
    PandaFramework* panda_framework_ = nullptr;
    WindowFramework* window_framework_ = nullptr;

    PT(AudioManager) music_manager_;
    bool app_has_audio_focus_ = true;
    bool music_manager_is_valid_ = false;
    bool music_active_ = false;
};

AsyncTask::DoneStatus ShowBase::Impl::audio_loop(GenericAsyncTask *task, void *user_data)
{
    Impl* impl = reinterpret_cast<Impl*>(user_data);

    if (impl->music_manager_)
    {
        impl->music_manager_->update();
    }
    //for x in self.sfxManagerList:
    //    x.update()
    return AsyncTask::DS_cont;
}

void ShowBase::Impl::create_base_audio_managers(void)
{
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
        throw_event_directly(*EventHandler::get_global_event_handler(), "MusicEnabled");
        rppanda_cat.debug() << "Enabling music" << std::endl;
    }
    else
    {
        rppanda_cat.debug() << "Disabling music" << std::endl;
    }
}

// ************************************************************************************************

ShowBase::ShowBase(PandaFramework* framework, WindowFramework* window_framework) : impl_(std::make_unique<Impl>())
{
    if (global_showbase)
    {
        rppanda_cat.error() << "ShowBase was already created!" << std::endl;
        return;
    }

    impl_->panda_framework_ = framework;
    impl_->window_framework_ = window_framework;

    /*
    window_framework = panda_framework->open_window();
    if (!window_framework)
        throw std::runtime_error("Cannot open Panda3D window!");
    */

    impl_->music_active_ = ConfigVariableBool("audio-music-active", true).get_value();
    want_render_2dp = ConfigVariableBool("want-render2dp", true).get_value();

    // If the aspect ratio is 0 or None, it means to infer the
    // aspect ratio from the window size.
    // If you need to know the actual aspect ratio call base.getAspectRatio()
    config_aspect_ratio = ConfigVariableDouble("aspect-ratio", 0).get_value();

    // This is set to the value of the window-type config variable, but may
    // optionally be overridden in the Showbase constructor.  Should either be
    // 'onscreen' (the default), 'offscreen' or 'none'.
    this->window_type = ConfigVariableString("window-type", "onscreen").get_value();
    require_window = ConfigVariableBool("require_window", true).get_value();

    // The global graphics engine, ie. GraphicsEngine.getGlobalPtr()
    graphics_engine = GraphicsEngine::get_global_ptr();
    setup_render();
    setup_data_graph();

    if (want_render_2dp)
        setup_render_2dp();

    win = window_framework->get_graphics_window();

    // Open the default rendering window.
    open_default_window();

    // The default is trackball mode, which is more convenient for
    // ad-hoc development in Python using ShowBase.Applications
    // can explicitly call base.useDrive() if they prefer a drive
    // interface.
    use_trackball();

    impl_->app_has_audio_focus_ = true;

    create_base_audio_managers();

    global_showbase = this;

    restart();
}

ShowBase::~ShowBase(void)
{
    shutdown();

    if (impl_->music_manager_)
    {
        impl_->music_manager_->shutdown();
        impl_->music_manager_.clear();
    }
    if (graphics_engine)
        graphics_engine->remove_all_windows();
}

ShowBase* ShowBase::get_global_ptr(void)
{
    return global_showbase;
}

PandaFramework* ShowBase::get_panda_framework(void) const
{
    return impl_->panda_framework_;
}

WindowFramework* ShowBase::get_window_framework(void) const
{
    return impl_->window_framework_;
}

AudioManager* ShowBase::get_music_manager(void) const
{
    return impl_->music_manager_;
}

NodePath ShowBase::get_render(void) const
{
    return impl_->window_framework_->get_render();
}

NodePath ShowBase::get_render_2d(void) const
{
    return impl_->window_framework_->get_render_2d();
}

NodePath ShowBase::get_aspect_2d(void) const
{
    return impl_->window_framework_->get_aspect_2d();
}

NodePath ShowBase::get_pixel_2d(void) const
{
    return impl_->window_framework_->get_pixel_2d();
}

AsyncTaskManager* ShowBase::get_task_mgr(void) const
{
    return AsyncTaskManager::get_global_ptr();
}

bool ShowBase::open_default_window(void)
{
    open_main_window();

    return win != nullptr;
}

void ShowBase::open_main_window(void)
{
    // framework.open_window()

    if (win)
    {
        setup_mouse();
        make_camera2dp(win);
    }
}

void ShowBase::setup_render_2dp(void)
{
    render_2dp = NodePath("render2dp");

    // Set up some overrides to turn off certain properties which
    // we probably won't need for 2-d objects.

    // It's probably important to turn off the depth test, since
    // many 2-d objects will be drawn over each other without
    // regard to depth position.

    const RenderAttrib* dt = DepthTestAttrib::make(DepthTestAttrib::M_none);
    const RenderAttrib* dw = DepthWriteAttrib::make(DepthWriteAttrib::M_off);
    render_2dp.set_depth_test(0);
    render_2dp.set_depth_write(0);

    render_2dp.set_material_off(1);
    render_2dp.set_two_sided(1);

    // The normal 2-d DisplayRegion has an aspect ratio that
    // matches the window, but its coordinate system is square.
    // This means anything we parent to render2dp gets stretched.
    // For things where that makes a difference, we set up
    // aspect2dp, which scales things back to the right aspect
    // ratio along the X axis (Z is still from -1 to 1)
    PT(PGTop) aspect_2dp_pg_top = new PGTop("aspect2dp");
    aspect_2dp = render_2dp.attach_new_node(aspect_2dp_pg_top);
    aspect_2dp_pg_top->set_start_sort(16384);

    const float aspect_ratio = get_aspect_ratio();
    aspect_2dp.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);

    // The Z position of the top border of the aspect2dp screen.
    a2dp_top = 1.0f;
    // The Z position of the bottom border of the aspect2dp screen.
    a2dp_bottom = -1.0f;
    // The X position of the left border of the aspect2dp screen.
    a2dp_left = -aspect_ratio;
    // The X position of the right border of the aspect2dp screen.
    a2dp_right = aspect_ratio;

    a2dp_top_center = aspect_2dp.attach_new_node("a2dpTopCenter");
    a2dp_bottom_center = aspect_2dp.attach_new_node("a2dpBottomCenter");
    a2dp_left_center = aspect_2dp.attach_new_node("a2dpLeftCenter");
    a2dp_right_center = aspect_2dp.attach_new_node("a2dpRightCenter");

    a2dp_top_left = aspect_2dp.attach_new_node("a2dpTopLeft");
    a2dp_top_right = aspect_2dp.attach_new_node("a2dpTopRight");
    a2dp_bottom_left = aspect_2dp.attach_new_node("a2dpBottomLeft");
    a2dp_bottom_right = aspect_2dp.attach_new_node("a2dpBottomRight");

    // Put the nodes in their places
    a2dp_top_center.set_pos(0, 0, a2dp_top);
    a2dp_bottom_center.set_pos(0, 0, a2dp_bottom);
    a2dp_left_center.set_pos(a2dp_left, 0, 0);
    a2dp_right_center.set_pos(a2dp_right, 0, 0);

    a2dp_top_left.set_pos(a2dp_left, 0, a2dp_top);
    a2dp_top_right.set_pos(a2dp_right, 0, a2dp_top);
    a2dp_bottom_left.set_pos(a2dp_left, 0, a2dp_bottom);
    a2dp_bottom_right.set_pos(a2dp_right, 0, a2dp_bottom);

    // This special root, pixel2d, uses units in pixels that are relative
    // to the window. The upperleft corner of the window is (0, 0),
    // the lowerleft corner is (xsize, -ysize), in this coordinate system.
    PT(PGTop) pixel_2dp_pg_top = new PGTop("pixel2dp");
    pixel_2dp = render_2dp.attach_new_node(pixel_2dp_pg_top);
    pixel_2dp_pg_top->set_start_sort(16384);
    pixel_2dp.set_pos(-1, 0, 1);
    const LVecBase2i& size = get_size();
    float xsize = size.get_x();
    float ysize = size.get_y();
    if (xsize > 0 && ysize > 0)
        pixel_2dp.set_scale(2.0f / xsize, 1.0f, 2.0f / ysize);
}

NodePath ShowBase::make_camera2dp(GraphicsWindow* win, int sort, const LVecBase4f& display_region,
    const LVecBase4f& coords, Lens* lens, const std::string& camera_name)
{
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
    if (camera2dp.is_empty())
        camera2dp = render_2dp.attach_new_node("camera2dp");

    camera2dp = camera2dp.attach_new_node(cam2d_node);
    dr->set_camera(camera2dp);

    return camera2dp;
}

void ShowBase::setup_data_graph(void)
{

}

void ShowBase::setup_mouse(void)
{
    setup_mouse_cb();

    mouse_watcher = impl_->window_framework_->get_mouse();
    mouse_watcher_node = DCAST(MouseWatcher, mouse_watcher.node());

    // In C++, aspect2d has already mouse watcher.
    DCAST(PGTop, aspect_2dp.node())->set_mouse_watcher(mouse_watcher_node);
    DCAST(PGTop, get_pixel_2d().node())->set_mouse_watcher(mouse_watcher_node);
    DCAST(PGTop, pixel_2dp.node())->set_mouse_watcher(mouse_watcher_node);
}

void ShowBase::setup_mouse_cb(void)
{
    // Note that WindowFramework::get_mouse

    impl_->window_framework_->enable_keyboard();
}

float ShowBase::get_aspect_ratio(GraphicsOutput* win) const
{
    // If the config it set, we return that
    if (config_aspect_ratio)
        return config_aspect_ratio;

    float aspect_ratio = 1.0f;

    if (!win)
        win = this->win;

    if (win && win->has_size() && win->get_sbs_left_y_size() != 0)
    {
        aspect_ratio = float(win->get_sbs_left_x_size()) / win->get_sbs_left_y_size();
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
            aspect_ratio = float(props.get_x_size()) / props.get_y_size();
    }

    return aspect_ratio == 0 ? 1 : aspect_ratio;
}

const LVecBase2i& ShowBase::get_size(GraphicsOutput* win) const
{
    if (!win)
        win = this->win;

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

        if (props.has_size())
        {
            return props.get_size();
        }
        else
        {
            static LVecBase2i zero_size(0);
            return zero_size;
        }
    }
}

NodePath ShowBase::get_camera(void) const
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

NodePath ShowBase::get_button_thrower(void) const
{
    // Node that WindowFramework::enable_keyboard
    return impl_->window_framework_->get_mouse().find("kb-events");
}

const NodePath& ShowBase::get_data_root(void) const
{
    return impl_->panda_framework_->get_data_root();
}

PandaNode* ShowBase::get_data_root_node(void) const
{
    return get_data_root().node();
}

void ShowBase::restart(void)
{
    shutdown();

    add_task(ival_loop, nullptr, "ival_loop", 20);

    // the audioLoop updates the positions of 3D sounds.
    // as such, it needs to run after the cull traversal in the igLoop.
    add_task(Impl::audio_loop, impl_.get(), "audio_loop", 60);
}

void ShowBase::shutdown(void)
{
    auto task_mgr = get_task_mgr();
    AsyncTask* task = nullptr;

    if (task = get_task_mgr()->find_task("audio_loop"))
        task_mgr->remove(task);
    if (task = get_task_mgr()->find_task("ival_loop"))
        task_mgr->remove(task);
}

void ShowBase::disable_mouse(void)
{
    // Note that WindowFramework::setup_trackball.
    NodePath tball2cam = impl_->window_framework_->get_mouse().find("trackball/tball2cam");
    if (!tball2cam.is_empty())
        tball2cam.detach_node();
}

void ShowBase::use_trackball(void)
{
    impl_->window_framework_->setup_trackball();
}

void ShowBase::create_base_audio_managers(void)
{
    impl_->create_base_audio_managers();
}

void ShowBase::enable_music(bool enable)
{
    impl_->enable_music(enable);
}

void ShowBase::disable_all_audio(void)
{
    impl_->app_has_audio_focus_ = false;
    // self.SetAllSfxEnables(0)
    if (impl_->music_manager_is_valid_)
        impl_->music_manager_->set_active(false);
    rppanda_cat.debug() << "Disabling audio" << std::endl;
}

void ShowBase::enable_all_audio(void)
{
    impl_->app_has_audio_focus_ = true;
    // self.SetAllSfxEnables(self.sfxActive)
    if (impl_->music_manager_is_valid_)
        impl_->music_manager_->set_active(impl_->music_active_);
    rppanda_cat.debug() << "Enabling audio" << std::endl;
}


AsyncTask::DoneStatus ShowBase::ival_loop(GenericAsyncTask *task, void *user_data)
{
    // Execute all intervals in the global ivalMgr.
    CIntervalManager::get_global_ptr()->step();
    return AsyncTask::DS_cont;
}

}
