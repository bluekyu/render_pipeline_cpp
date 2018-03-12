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

#include "render_pipeline/rppanda/interval/actor_interval.hpp"

#include <render_pipeline/rplibs/py_to_cpp.hpp>

#include "render_pipeline/rppanda/actor/actor.hpp"
#include "rppanda/interval/config_rppanda_interval.hpp"

namespace rppanda {

int ActorInterval::anim_num_ = 1;

TypeHandle ActorInterval::type_handle_;

ActorInterval::ActorInterval(Actor* actor, const std::vector<std::string>& anim_name, bool loop,
    bool constrained_loop, boost::optional<double> duration,
    boost::optional<double> start_time, boost::optional<double> end_time,
    boost::optional<double> start_frame, boost::optional<double> end_frame,
    double play_rate, const boost::optional<std::string> name, bool force_update,
    const std::vector<std::string>& part_name,
    const boost::optional<std::string>& lod_name): CInterval("unnamed", 0.0, false)
{
    // Generate unique id
    std::string id = "Actor-" + std::to_string(ActorInterval::anim_num_);
    ActorInterval::anim_num_ += 1;
    // Record class specific variables
    
    actor_ = actor;
    anim_name_ = anim_name;
    controls_ = actor_->get_anim_controls(anim_name_, part_name, lod_name);
    loop_anim_ = loop;
    constrained_loop_ = constrained_loop;
    force_update_ = force_update;
    _play_rate = play_rate;

    // If no name specified, use id as name
    _name = name ? name.value() : id;

    if (controls_.empty())
    {
        rppanda_interval_cat.warning() << "Unknown animation for actor" << std::endl;
        frame_rate_ = 1.0;
        start_frame_ = 0;
        end_frame_ = 0;
    }
    else
    {
        frame_rate_ = controls_[0]->get_frame_rate() * std::abs(_play_rate);
        // Compute start and end frames.
        if (start_frame)
            start_frame_ = start_frame.value();
        else if (start_time)
            start_frame_ = start_time.value() * frame_rate_;
        else
            start_frame_ = 0;

        if (end_frame)
        {
            end_frame_ = end_frame.value();
        }
        else if (end_time)
        {
            end_frame_ = end_time.value() * frame_rate_;
        }
        else if (duration)
        {
            if (!start_time)
                start_time = start_frame_ / frame_rate_;
            end_time = start_time.value() + duration.value();
            end_frame_ = end_time.value() * frame_rate_;
        }
        else
        {
            // No end frame specified.Choose the maximum of all
            // of the controls' numbers of frames.
            auto max_frames = controls_[0]->get_num_frames();
            bool warned = false;
            for (size_t k=0, k_end=controls_.size(); k < k_end; ++k)
            {
                auto num_frames = controls_[k]->get_num_frames();
                if (num_frames != max_frames && num_frames != 1 && !warned)
                {
                    rppanda_interval_cat.warning() << "Some animations on " << actor->get_name()
                        << " have an inconsistent number of frames." << std::endl;
                    warned = true;
                }
                max_frames = (std::max)(max_frames, num_frames);
            }
            end_frame_ = max_frames - 1;
        }
    }

    // Must we play the animation backwards?  We play backwards if
    // either(or both) of the following is true: the playRate is
    // negative, or endFrame is before startFrame.
    reverse_ = _play_rate < 0;
    if (end_frame_ < start_frame_)
    {
        reverse_ = true;
        std::swap(start_frame_, end_frame_);
    }

    num_frames_ = end_frame_ - start_frame_ + 1;

    // Compute duration if no duration specified
    implicit_duration_ = false;
    if (!duration)
    {
        implicit_duration_ = true;
        duration = num_frames_ / frame_rate_;
    }

    _duration = duration.value();
}

void ActorInterval::priv_step(double t)
{
    auto frame_count = t * frame_rate_;
    if (constrained_loop_)
        frame_count = rplibs::py_fmod(frame_count, num_frames_);

    double abs_frame;
    if (reverse_)
        abs_frame = end_frame_ - frame_count;
    else
        abs_frame = start_frame_ + frame_count;

    // Calc integer frame number
    auto int_frame = static_cast<int>(std::floor(abs_frame + 0.0001));

    // Pose anim

    // We use our pre-computed list of animControls for
    // efficiency's sake, rather than going through the relatively
    // expensive Actor interface every frame.
    for (auto&& control: controls_)
    {
        // Each animControl might have a different number of frames.
        auto num_frames = control->get_num_frames();
        double frame;
        if (loop_anim_)
            frame = (int_frame % num_frames) + (abs_frame - int_frame);
        else
            frame = (std::max)((std::min)(abs_frame, num_frames-1.0), 0.0);

        control->pose(frame);
    }

    if (force_update_)
        actor_->update();

    _state = CInterval::S_started;
    _curr_t = t;
}

void ActorInterval::priv_finalize()
{
    if (implicit_duration_ && !loop_anim_)
    {
        // As a special case, we ensure we end up posed to the last
        // frame of the animation if the original duration was
        // implicit.This is necessary only to guard against
        // possible roundoff error in computing the final frame
        // from the duration.We don't do this in the case of a
        // looping animation, however, because this would introduce
        // a hitch in the animation when it plays back-to-back with
        // the next cycle.
        if (reverse_)
        {
            for (auto&& control: controls_)
                control->pose(start_frame_);
        }
        else
        {
            for (auto&& control: controls_)
                control->pose(end_frame_);
        }
        if (force_update_)
            actor_->update();
    }
    else
    {
        // Otherwise, the user-specified duration determines which
        // is our final frame.
        priv_step(get_duration());
    }

    _state = CInterval::S_final;
    interval_done();
}

}
