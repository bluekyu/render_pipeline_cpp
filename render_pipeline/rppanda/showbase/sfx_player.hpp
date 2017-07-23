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
    register_type(type_handle_, "SfxPlayer", TypedReferenceCount::get_class_type());
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
