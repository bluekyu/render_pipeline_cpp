/**
 * Render Pipeline C++
 *
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
class RENDER_PIPELINE_DECL SfxPlayer: public TypedReferenceCount
{
public:
    static bool use_inverse_suqare_;

    SfxPlayer(void);
    ~SfxPlayer(void);

    /** Return the curent cutoff distance. */
    float get_cutoff_distance(void);

    float get_localized_volume(NodePath node, boost::optional<NodePath> listener_node={}, boost::optional<float> cutoff={});

    void set_cutoff_distance(float d);

    void play_sfx(AudioSound* sfx, bool looping=false, bool interrupt=true, boost::optional<float> volume={},
        float time=0.0, boost::optional<NodePath> node={}, boost::optional<NodePath> listener_node={},
        boost::optional<float> cutoff={});

    /** Calculate the final volume based on all contributed factors. */
    void set_final_volume(AudioSound* sfx, boost::optional<NodePath> node, boost::optional<float> volume,
        boost::optional<NodePath> listener_node, boost::optional<float> cutoff={});

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle SfxPlayer::get_class_type(void)
{
    return type_handle_;
}

inline void SfxPlayer::init_type(void)
{
    TypedReferenceCount::init_type();
    register_type(type_handle_, "rppanda::SfxPlayer", TypedReferenceCount::get_class_type());
}

inline TypeHandle SfxPlayer::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle SfxPlayer::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
