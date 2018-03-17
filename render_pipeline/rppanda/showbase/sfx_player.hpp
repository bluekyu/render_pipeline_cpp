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
 * Contains the SfxPlayer class, a thin utility class for playing sounds at
 * a particular location.
 *
 * This is C++ porting codes of direct/src/showbase/SfxPlayer.py
 */

#pragma once

#include <nodePath.h>

#include <memory>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

class AudioSound;

namespace rppanda {

/**
 * Play sound effects, potentially localized.
 */
class RENDER_PIPELINE_DECL SfxPlayer : public TypedReferenceCount
{
public:
    static bool use_inverse_suqare_;

    SfxPlayer();
    ~SfxPlayer();

    /** Return the curent cutoff distance. */
    float get_cutoff_distance();

    float get_localized_volume(NodePath node, boost::optional<NodePath> listener_node=boost::none, boost::optional<float> cutoff=boost::none);

    void set_cutoff_distance(float d);

    void play_sfx(AudioSound* sfx, bool looping=false, bool interrupt=true, boost::optional<float> volume=boost::none,
        float time=0.0, boost::optional<NodePath> node=boost::none, boost::optional<NodePath> listener_node=boost::none,
        boost::optional<float> cutoff=boost::none);

    /** Calculate the final volume based on all contributed factors. */
    void set_final_volume(AudioSound* sfx, boost::optional<NodePath> node, boost::optional<float> volume,
        boost::optional<NodePath> listener_node, boost::optional<float> cutoff=boost::none);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const override;
    virtual TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle SfxPlayer::get_class_type()
{
    return type_handle_;
}

inline void SfxPlayer::init_type()
{
    TypedReferenceCount::init_type();
    register_type(type_handle_, "rppanda::SfxPlayer", TypedReferenceCount::get_class_type());
}

inline TypeHandle SfxPlayer::get_type() const
{
    return get_class_type();
}

inline TypeHandle SfxPlayer::force_init_type()
{
    init_type();
    return get_class_type();
}

}
