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
 * SoundInterval module: contains the SoundInterval class.
 *
 * This is C++ porting codes of direct/src/interval/SoundInterval.py
 */

#pragma once

#include <audioSound.h>
#include <nodePath.h>
#include <cInterval.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL SoundInterval : public CInterval
{
public:
    static size_t sound_num_;   //!< Name counter

public:
    SoundInterval(AudioSound* sound, bool loop=false, double duration=0.0,
        const boost::optional<std::string>& name=boost::none, double volume=1.0, double start_time=0.0,
        boost::optional<NodePath> node=boost::none, bool seamless_loop=true, boost::optional<NodePath> listener_node=boost::none,
        boost::optional<float> cut_off=boost::none);

    ~SoundInterval() = default;

    AudioSound* get_sound() const;
    float get_sound_duration() const;
    bool get_f_loop() const;
    double get_volume() const;
    double get_start_time() const;
    boost::optional<NodePath> get_node() const;
    boost::optional<NodePath> get_listener_node() const;
    boost::optional<float> get_cut_off() const;

    void loop(double start_t=0.0, double end_t=-1.0, double play_rate=1.0, bool stagger=false);
    void finish();

    void priv_initialize(double t) override;
    void priv_instant() override;
    void priv_step(double t) override;
    void priv_finalize() override;
    void priv_reverse_initialize(double t) override;
    void priv_reverse_instant() override;
    void priv_reverse_finalize() override;
    void priv_interrupt() override;

protected:
    PT(AudioSound) sound_;
    float sound_duration_;
    bool f_loop_;
    double volume_;
    double start_time_;
    boost::optional<NodePath> node_;
    boost::optional<NodePath> listener_node_;
    boost::optional<float> cut_off_;
    bool seamless_loop_;
    bool sound_playing_;
    bool reverse_;
    bool in_finish_ = false;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline AudioSound* SoundInterval::get_sound() const
{
    return sound_;
}

inline float SoundInterval::get_sound_duration() const
{
    return sound_duration_;
}

inline bool SoundInterval::get_f_loop() const
{
    return f_loop_;
}

inline double SoundInterval::get_volume() const
{
    return volume_;
}

inline double SoundInterval::get_start_time() const
{
    return start_time_;
}

inline boost::optional<NodePath> SoundInterval::get_node() const
{
    return node_;
}

inline boost::optional<NodePath> SoundInterval::get_listener_node() const
{
    return listener_node_;
}

inline boost::optional<float> SoundInterval::get_cut_off() const
{
    return cut_off_;
}

inline TypeHandle SoundInterval::get_class_type()
{
    return type_handle_;
}

inline void SoundInterval::init_type()
{
    CInterval::init_type();
    register_type(type_handle_, "rppanda::SoundInterval", CInterval::get_class_type());
}

inline TypeHandle SoundInterval::get_type() const
{
    return get_class_type();
}

inline TypeHandle SoundInterval::force_init_type()
{
    init_type();
    return get_class_type();
}

}
