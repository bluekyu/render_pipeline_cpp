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
 * This is C++ porting codes of direct/src/showbase/SfxPlayer.py
 */

#include "render_pipeline/rppanda/showbase/sfx_player.hpp"

#include <audioSound.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rppanda {

bool SfxPlayer::use_inverse_suqare_ = false;

class SfxPlayer::Impl
{
public:
    void set_cutoff_distance(float d);

    float cutoff_volume_;
    float cutoff_distance_;
    float raw_cutoff_distance_;
    float distance_scale_;
};

void SfxPlayer::Impl::set_cutoff_distance(float d)
{
    cutoff_distance_ = d;

    // this is the 'raw' distance at which the volume of a sound will
    // be equal to the cutoff volume
    raw_cutoff_distance_ = std::sqrt(1. / cutoff_volume_);

    // this is a scale factor to convert distances so that a sound
    // located at self.cutoffDistance will have a volume
    // of self.cutoffVolume
    distance_scale_ = raw_cutoff_distance_ / cutoff_distance_;
}

SfxPlayer::SfxPlayer(): impl_(std::make_unique<Impl>())
{
    // volume attenuates according to the inverse square of the
    // distance from the source. volume = 1/(distance^2)
    // this is the volume at which a sound is nearly inaudible
    impl_->cutoff_volume_ = 0.02;

    // cutoff for inverse square attenuation
    if (use_inverse_suqare_)
    {
        set_cutoff_distance(300.0f);
    }
    else
    {
        // Distance at which sounds can no longer be heard
        // This was determined experimentally
        set_cutoff_distance(120.0f);
    }
}

SfxPlayer::~SfxPlayer() = default;

float SfxPlayer::get_cutoff_distance()
{
    return impl_->cutoff_distance_;
}

float SfxPlayer::get_localized_volume(NodePath node, boost::optional<NodePath> listener_node, boost::optional<float> cutoff)
{
    if (node.is_empty())
        return 0;

    float d;
    if (listener_node && listener_node.value().is_empty())
        d = node.get_distance(listener_node.value());
    else
        d = node.get_distance(ShowBase::get_global_ptr()->get_cam());

    if (cutoff && d > cutoff.value())
        return 0;

    if (use_inverse_suqare_)
    {
        float sd = d * impl_->distance_scale_;
        return (std::min)(1.0f, 1.0f / (sd ? sd*sd : 1.0f));
    }
    else
    {
        return 1 - (d / (cutoff ? cutoff.value() : 1.0f));
    }
}

void SfxPlayer::set_cutoff_distance(float d)
{
    impl_->set_cutoff_distance(d);
}

void SfxPlayer::play_sfx(AudioSound* sfx, bool looping, bool interrupt, boost::optional<float> volume,
    float time, boost::optional<NodePath> node, boost::optional<NodePath> listener_node,
    boost::optional<float> cutoff)
{
    if (!sfx)
        return;

    if (!cutoff)
        cutoff = impl_->cutoff_distance_;

    set_final_volume(sfx, node, volume, listener_node, cutoff.value());

    if (interrupt || (sfx->status() != AudioSound::PLAYING))
    {
        sfx->set_time(time);
        sfx->set_loop(looping);
        sfx->play();
    }
}

void SfxPlayer::set_final_volume(AudioSound* sfx, boost::optional<NodePath> node, boost::optional<float> volume,
    boost::optional<NodePath> listener_node, boost::optional<float> cutoff)
{
    // If we have either a node or a volume, we need to adjust the sfx
    // The volume passed in multiplies the distance base volume

    if (!node && !volume)
        return;

    float final_volume;
    if (node)
        final_volume = get_localized_volume(node.value(), listener_node, cutoff);
    else
        final_volume = 1.0f;

    if (volume)
        final_volume *= volume.value();

    if (node)
        final_volume *= node.value().get_net_audio_volume();

    sfx->set_volume(final_volume);
}

}
