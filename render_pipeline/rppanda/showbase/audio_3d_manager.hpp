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
 * This is C++ porting codes of direct/src/showbase/Audio3DManager.py
 */

#pragma once

#include <unordered_map>

#include <weakNodePath.h>
#include <nodePath.h>
#include <asyncTask.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

class AudioManager;
class AudioSound;

namespace rppanda {

class RENDER_PIPELINE_DECL Audio3DManager
{
public:
    Audio3DManager(AudioManager* audio_manager, NodePath listener_target = NodePath{}, NodePath root = NodePath{}, int task_priority = 51);

    ~Audio3DManager();

    /**
     * Use Audio3DManager.loadSfx to load a sound with 3D positioning enabled.
     */
    PT(AudioSound) load_sfx(const Filename& name);

    /**
     * Control the scale that sets the distance units for 3D spacialized audio.
     * Default is 1.0 which is adjust in panda to be feet.
     * When you change this, don't forget that this effects the scale of setSoundMinDistance
     */
    void set_distance_factor(PN_stdfloat factor) const;

    /**
     * Control the scale that sets the distance units for 3D spacialized audio.
     * Default is 1.0 which is adjust in panda to be feet.
     */
    PN_stdfloat get_distance_factor() const;

    /**
     * Control the presence of the Doppler effect. Default is 1.0
     * Exaggerated Doppler, use >1.0
     * Diminshed Doppler, use <1.0
     */
    void set_doppler_factor(PN_stdfloat factor);

    /**
     * Control the presence of the Doppler effect. Default is 1.0
     * Exaggerated Doppler, use >1.0
     * Diminshed Doppler, use <1.0
     */
    PN_stdfloat get_doppler_factor() const;

    /**
     * Exaggerate or diminish the effect of distance on sound. Default is 1.0
     * Valid range is 0 to 10
     * Faster drop off, use >1.0
     * Slower drop off, use <1.0
     */
    void set_drop_off_factor(PN_stdfloat factor);

    /**
     * Exaggerate or diminish the effect of distance on sound. Default is 1.0
     * Valid range is 0 to 10
     * Faster drop off, use >1.0
     * Slower drop off, use <1.0
     */
    PN_stdfloat get_drop_off_factor() const;

    /**
     * Controls the distance (in units) that this sound begins to fall off.
     * Also affects the rate it falls off.
     * Default is 3.28 (in feet, this is 1 meter)
     * Don't forget to change this when you change the DistanceFactor
     */
    void set_sound_min_distance(AudioSound* sound, PN_stdfloat dist);

    /**
     * Controls the distance (in units) that this sound begins to fall off.
     * Also affects the rate it falls off.
     * Default is 3.28 (in feet, this is 1 meter)
     */
    PN_stdfloat get_sound_min_distance(AudioSound* sound) const;

    /**
     * Controls the maximum distance (in units) that this sound stops falling off.
     * The sound does not stop at that point, it just doesn't get any quieter.
     * You should rarely need to adjust this.
     * Default is 1000000000.0
     */
    void set_sound_max_distance(AudioSound* sound, PN_stdfloat dist);

    /**
     * Controls the maximum distance (in units) that this sound stops falling off.
     * The sound does not stop at that point, it just doesn't get any quieter.
     * You should rarely need to adjust this.
     * Default is 1000000000.0
     */
    PN_stdfloat get_sound_max_distance(AudioSound* sound) const;

    /**
     * Set the velocity vector (in units/sec) of the sound, for calculating doppler shift.
     * This is relative to the sound root (probably render).
     * Default: LVecBase3(0, 0, 0)
     */
    void set_sound_velocity(AudioSound* sound, const LVecBase3& velocity);

    /**
     * If velocity is set to auto, the velocity will be determined by the
     * previous position of the object the sound is attached to and the frame dt.
     * Make sure if you use this method that you remember to clear the previous
     * transformation between frames.
     */
    void set_sound_velocity_auto(AudioSound* sound);

    /** Get the velocity of the sound. */
    LVecBase3 get_sound_velocity(AudioSound* sound);

    /**
     * Set the velocity vector (in units/sec) of the listener, for calculating doppler shift.
     * This is relative to the sound root (probably render).
     * Default: LVecBase3(0, 0, 0)
     */
    void set_listener_velocity(const LVecBase3& velocity);

    /**
     * If velocity is set to auto, the velocity will be determined by the
     * previous position of the object the listener is attached to and the frame dt.
     * Make sure if you use this method that you remember to clear the previous
     * transformation between frames.
     */
    void set_listener_velocity_auto();

    /** Get the velocity of the listener. */
    LVecBase3 get_listener_velocity() const;

    /**
     * Sound will come from the location of the object it is attached to.
     * If the object is deleted, the sound will automatically be removed.
     */
    bool attach_sound_to_object(AudioSound* sound, NodePath object);

    /**
     * sound will no longer have it's 3D position updated.
     */
    bool detach_sound(AudioSound* sound);

    /**
     * returns a list of sounds attached to an object.
     */
    const std::vector<AudioSound*>& get_sounds_on_object(NodePath object) const;

    /**
     * Sounds will be heard relative to this object. Should probably be the camera.
     */
    bool attach_listener(NodePath object);

    /**
     * Sounds will be heard relative to the root, probably render.
     */
    bool detach_listener();

    /**
     * Updates position of sounds in the 3D audio system. Will be called automatically
     * in a task.
     */
    AsyncTask::DoneStatus update(AsyncTask* task = nullptr);

    /**
     * Detaches any existing sounds and removes the update task.
     */
    void disable();

private:
    AudioManager* audio_manager_;
    NodePath listener_target_;
    NodePath root_;

    boost::optional<LVecBase3> listener_vel_;

    std::unordered_map<AudioSound*, boost::optional<LVecBase3>> vel_dict_;
    std::map<WeakNodePath, std::vector<AudioSound*>> sound_dict_;
};

// ************************************************************************************************

inline void Audio3DManager::set_sound_velocity(AudioSound* sound, const LVecBase3& velocity)
{
    vel_dict_[sound] = velocity;
}

inline void Audio3DManager::set_sound_velocity_auto(AudioSound* sound)
{
    vel_dict_[sound].reset();
}

inline void Audio3DManager::set_listener_velocity(const LVecBase3& velocity)
{
    listener_vel_ = velocity;
}

inline void Audio3DManager::set_listener_velocity_auto()
{
    listener_vel_.reset();
}

inline bool Audio3DManager::attach_listener(NodePath object)
{
    listener_target_ = object;
    return true;
}

inline bool Audio3DManager::detach_listener()
{
    listener_target_ = NodePath{};
    return true;
}

}
