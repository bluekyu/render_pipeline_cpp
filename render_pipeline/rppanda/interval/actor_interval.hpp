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

#pragma once

#include <cInterval.h>

#include <boost/optional.hpp>

class AnimControl;

namespace rppanda {

class Actor;

class ActorInterval : public CInterval
{
public:
    // Name counter
    static int anim_num_;

public:
    ActorInterval(Actor* actor, const std::vector<std::string>& anim_name, bool loop=false,
        bool constrained_loop=false, boost::optional<double> duration=boost::none,
        boost::optional<double> start_time=boost::none, boost::optional<double> end_time=boost::none,
        boost::optional<double> start_frame=boost::none, boost::optional<double> end_frame=boost::none,
        double play_rate=1.0, const boost::optional<std::string> name=boost::none, bool force_update=false,
        const std::vector<std::string>& part_name={}, const boost::optional<std::string>& lod_name=boost::none);

    void priv_step(double t) override;
    void priv_finalize() override;

private:
    WPT(Actor) actor_;
    std::vector<std::string> anim_name_;
    std::vector<AnimControl*> controls_;
    bool loop_anim_;
    bool constrained_loop_;
    bool force_update_;

    double frame_rate_;
    double start_frame_;
    double end_frame_;

    bool reverse_;

    double num_frames_;

    bool implicit_duration_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle ActorInterval::get_class_type()
{
    return type_handle_;
}

inline void ActorInterval::init_type()
{
    CInterval::init_type();
    register_type(type_handle_, "rppanda::ActorInterval", CInterval::get_class_type());
}

inline TypeHandle ActorInterval::get_type() const
{
    return get_class_type();
}

inline TypeHandle ActorInterval::force_init_type()
{
    init_type();
    return get_class_type();
}

}
