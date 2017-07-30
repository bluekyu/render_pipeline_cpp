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

#pragma once

#include <luse.h>

#include <render_pipeline/rpcore/config.hpp>

class ClockObject;

namespace rppanda {
class ShowBase;
}

namespace rpcore {

/**
 * This is a helper class, used to controll the camera and enable various
 * debugging features. It is not really part of the pipeline, but included to
 * view the demo scenes.
 */
class RENDER_PIPELINE_DECL MovementController
{
public:
    using MotionPathType = std::vector<std::pair<LVecBase3, LVecBase3>>;

public:
    MovementController(rppanda::ShowBase* showbase);

    MovementController(const MovementController&) = delete;
    MovementController(MovementController&&) = delete;

    ~MovementController(void);

    MovementController& operator=(const MovementController&) = delete;
    MovementController& operator=(MovementController&&) = delete;

    /** Sets the initial camera position. */
    void set_initial_position(const LVecBase3& pos, const LVecBase3& target);

    /** Sets the initial camera position. */
    void set_initial_position_hpr(const LVecBase3& pos, const LVecBase3& hpr);

    /** Resets the camera to the initial position. */
    void reset_to_initial(void);

    float get_speed(void) const;

    void set_speed(float speed);
    void set_bobbing_amount(float amount);
    void set_bobbing_speed(float speed);

    void set_movement(int direction, float amount);

    void set_hpr_movement(int direction, float amount);

    void set_mouse_enabled(bool enabled);

    void increase_speed(void);
    void decrease_speed(void);

    void increase_bobbing_amount(void);
    void decrease_bobbing_amount(void);

    void increase_bobbing_speed(void);
    void decrease_bobbing_speed(void);

    ClockObject* get_clock_obj(void);

    void setup(void);

    /** Prints the camera position and hpr. */
    void print_position(void);

    /** Plays a motion path from the given set of points. */
    void play_motion_path(const MotionPathType& points, float point_duration=1.2f);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
