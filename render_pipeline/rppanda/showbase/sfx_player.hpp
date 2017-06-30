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
class RENDER_PIPELINE_DECL SfxPlayer
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
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
