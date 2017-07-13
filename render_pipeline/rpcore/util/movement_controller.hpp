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
    ~MovementController(void);

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
