/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "render_pipeline/rpcore/util/movement_controller.hpp"

#include <fmt/format.h>

#include <parametricCurveCollection.h>
#include <mouseWatcher.h>
#include <buttonThrower.h>
#include <asyncTaskManager.h>
#include <graphicsWindow.h>
#include <curveFitter.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"
#include "render_pipeline/rplibs/py_to_cpp.hpp"

namespace rpcore {

class MovementController::Impl
{
public:
    Impl(rppanda::ShowBase* showbase);

    /** Internal update method. */
    AsyncTask::DoneStatus update(MovementController* self);

    AsyncTask::DoneStatus camera_motion_update(MovementController* self);

public:
    rppanda::ShowBase* showbase_;

    LVecBase3 movement_ = LVecBase3(0);
    LVecBase3 velocity_ = LVecBase3(0);
    LVecBase2 hpr_movement_ = LVecBase2(0);
    float speed_ = 0.4f;
    LVecBase3 initial_position_ = LVecBase3(0);
    LVecBase3 initial_destination_ = LVecBase3(0);
    LVecBase3 initial_hpr_ = LVecBase3(0);
    bool mouse_enabled_ = false;
    LVecBase2 last_mouse_pos_ = LVecBase2(0);
    float mouse_sensivity_ = 0.7f;
    float keyboard_hpr_speed_ = 0.4f;
    bool use_hpr_ = false;
    float smoothness_ = 6.0f;
    float bobbing_amount_ = 1.5f;
    float bobbing_speed_ = 0.5f;
    LVecBase2 current_mouse_pos_ = LVecBase2(0);

    PT(AsyncTask) update_task_;

    PT(ParametricCurveCollection) curve_;
    double curve_time_start_;
    double curve_time_end_;
    double delta_time_sum_ = 0;
    double delta_time_count_;
};

AsyncTask::DoneStatus MovementController::Impl::update(MovementController* self)
{
    double delta = self->get_clock_obj()->get_dt();

    NodePath camera = showbase_->get_camera();

    // Update mouse first
    if (showbase_->get_mouse_watcher_node()->has_mouse())
    {
        const LVecBase2& mouse_pos = showbase_->get_mouse_watcher_node()->get_mouse();

        current_mouse_pos_ = LVecBase2(
            mouse_pos[0] * showbase_->get_cam_lens()->get_fov().get_x(),
            mouse_pos[1] * showbase_->get_cam_lens()->get_fov().get_y()) * mouse_sensivity_;

        if (mouse_enabled_)
        {
            float diffx = last_mouse_pos_[0] - current_mouse_pos_[0];
            float diffy = last_mouse_pos_[1] - current_mouse_pos_[1];

            // Don't move in the very beginning
            if (last_mouse_pos_[0] == 0 && last_mouse_pos_[1] == 0)
            {
                diffx = 0;
                diffy = 0;
            }

            camera.set_h(camera.get_h() + diffx);
            camera.set_p(camera.get_p() - diffy);
        }

        last_mouse_pos_ = current_mouse_pos_;
    }

    // Compute movement in render space
    const LVecBase3& movement_direction = LVecBase3(movement_[1], movement_[0], 0.0f) * speed_ * delta * 100.0f;

    // transform by the camera direction
    const LQuaternionf camera_quaternion(camera.get_quat(showbase_->get_render()));
    LVecBase3 translated_direction(camera_quaternion.xform(movement_direction));

    // z-force is inddpendent of camera direction
    translated_direction.add_z(movement_[2] * delta * 120.0f * speed_);

    velocity_ += translated_direction * 0.15f;

    // apply the new position
    camera.set_pos(camera.get_pos() + velocity_);

    // transform rotation (keyboard keys)
    float rotation_speed = keyboard_hpr_speed_ * 100.0f;
    rotation_speed *= delta;
    camera.set_hpr(camera.get_hpr() +
        LVecBase3(hpr_movement_[0], hpr_movement_[1], 0.0f) * rotation_speed);

    // fade out velocity
    velocity_ = velocity_ * (std::max)(0.0, 1.0f - delta * 60.0f / (std::max)(0.01f, smoothness_));

    // bobbing
    const float ftime = static_cast<float>(self->get_clock_obj()->get_frame_time());
    float rotation = rplibs::py_fmod(ftime, bobbing_speed_) / bobbing_speed_;
    rotation = ((std::min)(rotation, 1.0f - rotation) * 2.0f - 0.5f) * 2.0f;
    if (velocity_.length_squared() > 1e-5 && speed_ > 1e-5)
    {
        rotation *= bobbing_amount_;
        rotation *= (std::min)(1.0f, velocity_.length()) / speed_ * 0.5f;
    }
    else
    {
        rotation = 0.0f;
    }
    camera.set_r(rotation);

    return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus MovementController::Impl::camera_motion_update(MovementController* self)
{
    if (self->get_clock_obj()->get_frame_time() > curve_time_end_)
    {
        std::cout << "Camera motion path finished" << std::endl;

        // Print performance stats
        const double avg_ms = delta_time_sum_ / delta_time_count_;
        std::cout << fmt::format("Average frame time (ms): {:4.1f}", avg_ms * 1000.0) << std::endl;
        std::cout << fmt::format("Average frame rate: {:4.1f}", 1.0 / avg_ms) << std::endl;

        update_task_ = self->add_task([this, self](rppanda::FunctionalTask* task) {
            return update(self);
        }, "RP_UpdateMovementController", -50);
        showbase_->get_render_2d().show();
        showbase_->get_aspect_2d().show();

        return AsyncTask::DS_done;
    }

    double lerp = (self->get_clock_obj()->get_frame_time() - curve_time_start_) / (curve_time_end_ - curve_time_start_);
    lerp *= curve_->get_max_t();

    LPoint3 pos(0);
    LVecBase3 hpr(0);
    curve_->evaluate_xyz(lerp, pos);
    curve_->evaluate_hpr(lerp, hpr);

    showbase_->get_camera().set_pos_hpr(pos, hpr);

    delta_time_sum_ += self->get_clock_obj()->get_dt();
    delta_time_count_ += 1;

    return AsyncTask::DS_cont;
}

MovementController::Impl::Impl(rppanda::ShowBase* showbase): showbase_(showbase)
{
}

// ************************************************************************************************
MovementController::MovementController(rppanda::ShowBase* showbase): impl_(std::make_unique<Impl>(showbase))
{
}

MovementController::~MovementController() = default;

void MovementController::reset_to_initial()
{
    NodePath camera = impl_->showbase_->get_camera();
    camera.set_pos(impl_->initial_position_);

    if (impl_->use_hpr_)
    {
        camera.set_hpr(impl_->initial_hpr_);
    }
    else
    {
        camera.look_at(impl_->initial_destination_.get_x(), impl_->initial_destination_.get_y(), impl_->initial_destination_.get_z());
    }
}

ClockObject* MovementController::get_clock_obj() const
{
    return impl_->showbase_->get_task_mgr()->get_global_clock();
}

void MovementController::setup()
{
    auto showbase = impl_->showbase_;

    // x
    accept("raw-w", [this](const Event*) { set_movement(0, 1); });
    accept("raw-w-up", [this](const Event*) { set_movement(0, 0); });
    accept("raw-s", [this](const Event*) { set_movement(0, -1); });
    accept("raw-s-up", [this](const Event*) { set_movement(0, 0); });

    // y
    accept("raw-a", [this](const Event*) { set_movement(1, -1); });
    accept("raw-a-up", [this](const Event* ev) { set_movement(1, 0); });
    accept("raw-d", [this](const Event* ev) { set_movement(1, 1); });
    accept("raw-d-up", [this](const Event* ev) { set_movement(1, 0); });

    // z
    accept("space", [this](const Event* ev) { set_movement(2, 1); });
    accept("space-up", [this](const Event* ev) { set_movement(2, 0); });
    accept("shift", [this](const Event* ev) { set_movement(2, -1); });
    accept("shift-up", [this](const Event* ev) { set_movement(2, 0); });

    // wireframe + debug + buffer viewer
    accept("f3", [this](const Event* ev) { impl_->showbase_->toggle_wireframe(); });
    accept("f11", [this](const Event* ev) { impl_->showbase_->get_win()->save_screenshot("screenshot.png"); });
    accept("j", [this](const Event* ev) { print_position(); });

    // mouse
    accept("mouse1", [this](const Event* ev) { set_mouse_enabled(true); });
    accept("mouse1-up", [this](const Event* ev) { set_mouse_enabled(false); });

    // arrow mouse navigation
    accept("arrow_up", [this](const Event* ev) { set_hpr_movement(1, 1); });
    accept("arrow_up-up", [this](const Event* ev) { set_hpr_movement(1, 0); });
    accept("arrow_down", [this](const Event* ev) { set_hpr_movement(1, -1); });
    accept("arrow_down-up", [this](const Event* ev) { set_hpr_movement(1, 0); });
    accept("arrow_left", [this](const Event* ev) { set_hpr_movement(0, 1); });
    accept("arrow_left-up", [this](const Event* ev) { set_hpr_movement(0, 0); });
    accept("arrow_right", [this](const Event* ev) { set_hpr_movement(0, -1); });
    accept("arrow_right-up", [this](const Event* ev) { set_hpr_movement(0, 0); });

    // increase / decrease speed
    accept("+", [this](const Event* ev) { increase_speed(); });
    accept("-", [this](const Event* ev) { decrease_speed(); });

    // disable modifier buttons to be able to move while pressing shift for example
    showbase->get_mouse_watcher_node()->set_modifier_buttons(ModifierButtons());
    DCAST(ButtonThrower, showbase->get_button_thrower().node())->set_modifier_buttons(ModifierButtons());

    // disable pandas builtin mouse control
    showbase->disable_mouse();

    // add ourself as an update task which gets executed very early before the rendering
    impl_->update_task_ = add_task([this](rppanda::FunctionalTask* task) {
        return impl_->update(this);
    }, "RP_UpdateMovementController", -50);

    // Hotkeys to connect to pstats and reset the initial position
    accept("1", [](const Event* ev) { PStatClient::connect(); });
    accept("3", [this](const Event* ev) { reset_to_initial(); });
}

void MovementController::print_position()
{
    const LVecBase3f& pos = impl_->showbase_->get_cam().get_pos(impl_->showbase_->get_render());
    const LVecBase3f& hpr = impl_->showbase_->get_cam().get_hpr(impl_->showbase_->get_render());
    printf("(Vec3(%f, %f, %f), Vec3(%f, %f, %f))\n", pos.get_x(), pos.get_y(), pos.get_z(), hpr.get_x(), hpr.get_y(), hpr.get_z());
}

void MovementController::play_motion_path(const MotionPathType& points, float point_duration)
{
    CurveFitter fitter;
    for (size_t k=0, k_end=points.size(); k < k_end; ++k)
        fitter.add_xyz_hpr(k, points[k].first, points[k].second);

    fitter.compute_tangents(1.0f);
    PT(ParametricCurveCollection) curve = fitter.make_hermite();
    std::cout << "Starting motion path with " << points.size() << " CVs" << std::endl;

    impl_->showbase_->get_render_2d().hide();
    impl_->showbase_->get_aspect_2d().hide();

    impl_->curve_ = curve;
    impl_->curve_time_start_ = get_clock_obj()->get_frame_time();
    impl_->curve_time_end_ = get_clock_obj()->get_frame_time() + points.size() * point_duration;
    impl_->delta_time_sum_ = 0;
    impl_->delta_time_count_ = 0;
    add_task([this](rppanda::FunctionalTask* task) {
        return impl_->camera_motion_update(this);
    }, "RP_CameraMotionPath", -50);
    impl_->update_task_->remove();
    impl_->update_task_.clear();
}

void MovementController::set_initial_position(const LVecBase3& pos, const LVecBase3& target)
{
    impl_->initial_position_ = pos;
    impl_->initial_destination_ = target;
    impl_->use_hpr_ = false;
    reset_to_initial();
}

void MovementController::set_initial_position_hpr(const LVecBase3& pos, const LVecBase3& hpr)
{
    impl_->initial_position_ = pos;
    impl_->initial_hpr_ = hpr;
    impl_->use_hpr_ = true;
    reset_to_initial();
}

float MovementController::get_speed() const
{
    return impl_->speed_;
}

void MovementController::set_movement(int direction, float amount)
{
    impl_->movement_[direction] = amount;
}

void MovementController::set_hpr_movement(int direction, float amount)
{
    impl_->hpr_movement_[direction] = amount;
}

void MovementController::set_mouse_enabled(bool enabled)
{
    impl_->mouse_enabled_ = enabled;
}

void MovementController::increase_speed()
{
    impl_->speed_ *= 1.4f;
}

void MovementController::decrease_speed()
{
    impl_->speed_ *= 0.6f;
}

void MovementController::set_speed(float speed)
{
    impl_->speed_ = speed;
}

void MovementController::increase_bobbing_amount()
{
    impl_->bobbing_amount_ *= 1.4f;
}

void MovementController::decrease_bobbing_amount()
{
    impl_->bobbing_amount_ *= 0.6f;
}

void MovementController::increase_bobbing_speed()
{
    impl_->bobbing_speed_ *= 1.4f;
}

void MovementController::decrease_bobbing_speed()
{
    impl_->bobbing_speed_ *= 0.6f;
}

void MovementController::set_bobbing_amount(float amount)
{
    impl_->bobbing_amount_ = amount;
}

void MovementController::set_bobbing_speed(float speed)
{
    impl_->bobbing_speed_ = speed;
}

}
