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

#include "render_pipeline/rppanda/showbase/audio_3d_manager.hpp"

#include <audioManager.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/task/task_manager.hpp"

namespace rppanda {

Audio3DManager::Audio3DManager(AudioManager* audio_manager, NodePath listener_target,
    NodePath root, int task_priority) : audio_manager_(audio_manager), listener_target_(listener_target)
{
    if (root.is_empty())
    {
        root_ = ShowBase::get_global_ptr()->get_render();
    }
    else
    {
        root_ = root;
    }

    listener_vel_ = LVecBase3(0);

    ShowBase::get_global_ptr()->get_task_mgr()->add([this](auto) { return update(); }, "Audio3DManager-updateTask", task_priority);
}

Audio3DManager::~Audio3DManager()
{
    disable();
}

PT(AudioSound) Audio3DManager::load_sfx(const Filename& name)
{
    return audio_manager_->get_sound(name, true);
}

void Audio3DManager::set_distance_factor(PN_stdfloat factor) const
{
    audio_manager_->audio_3d_set_distance_factor(factor);
}

PN_stdfloat Audio3DManager::get_distance_factor() const
{
    return audio_manager_->audio_3d_get_distance_factor();
}

void Audio3DManager::set_doppler_factor(PN_stdfloat factor)
{
    audio_manager_->audio_3d_set_doppler_factor(factor);
}

PN_stdfloat Audio3DManager::get_doppler_factor() const
{
    return audio_manager_->audio_3d_get_doppler_factor();
}

void Audio3DManager::set_drop_off_factor(PN_stdfloat factor)
{
    audio_manager_->audio_3d_set_drop_off_factor(factor);
}

PN_stdfloat Audio3DManager::get_drop_off_factor() const
{
    return audio_manager_->audio_3d_get_drop_off_factor();
}

void Audio3DManager::set_sound_min_distance(AudioSound* sound, PN_stdfloat dist)
{
    sound->set_3d_min_distance(dist);
}

PN_stdfloat Audio3DManager::get_sound_min_distance(AudioSound* sound) const
{
    return sound->get_3d_min_distance();
}

void Audio3DManager::set_sound_max_distance(AudioSound* sound, PN_stdfloat dist)
{
    sound->set_3d_max_distance(dist);
}

PN_stdfloat Audio3DManager::get_sound_max_distance(AudioSound* sound) const
{
    return sound->get_3d_max_distance();
}

LVecBase3 Audio3DManager::get_sound_velocity(AudioSound* sound)
{
    auto found = vel_dict_.find(sound);
    if (found != vel_dict_.end())
    {
        auto vel = found->second;
        if (vel)
            return *vel;

        for (auto&& sound_dict_pair : sound_dict_)
        {
            auto& sounds = sound_dict_pair.second;

            auto found = std::find(sounds.begin(), sounds.end(), sound);
            if (found != sounds.end())
            {
                auto& known_object = sound_dict_pair.first;
                auto node_path = known_object.get_node_path();
                if (!node_path)
                {
                    // The node has been deleted.
                    sound_dict_.erase(known_object);
                    continue;
                }

                return node_path.get_pos_delta(root_) / ClockObject::get_global_clock()->get_dt();
            }
        }
    }

    return LVecBase3(0);
}

LVecBase3 Audio3DManager::get_listener_velocity() const
{
    if (listener_vel_)
    {
        return *listener_vel_;
    }
    else if (listener_target_)
    {
        return listener_target_.get_pos_delta(root_) / ClockObject::get_global_clock()->get_dt();
    }
    else
    {
        return LVecBase3(0);
    }
}

bool Audio3DManager::attach_sound_to_object(AudioSound* sound, NodePath object)
{
    for (auto&& sound_dict_pair : sound_dict_)
    {
        auto& sounds = sound_dict_pair.second;

        auto found = std::find(sounds.begin(), sounds.end(), sound);
        if (found != sounds.end())
        {
            sounds.erase(found);
            if (sounds.empty())
            {
                auto& known_object = sound_dict_pair.first;

                // if there are no other sounds, don't track
                // the object any more
                sound_dict_.erase(known_object);
            }
        }
    }

    sound_dict_[WeakNodePath(object)].push_back(sound);

    return true;
}

bool Audio3DManager::detach_sound(AudioSound* sound)
{
    for (auto&& sound_dict_pair : sound_dict_)
    {
        auto& sounds = sound_dict_pair.second;

        auto found = std::find(sounds.begin(), sounds.end(), sound);
        if (found != sounds.end())
        {
            sounds.erase(found);
            if (sounds.empty())
            {
                auto& known_object = sound_dict_pair.first;

                // if there are no other sounds, don't track
                // the object any more
                sound_dict_.erase(known_object);
            }
            return true;
        }
    }
    return false;
}

const std::vector<AudioSound*>& Audio3DManager::get_sounds_on_object(NodePath object) const
{
    auto found = sound_dict_.find(WeakNodePath(object));
    if (found != sound_dict_.end())
    {
        return found->second;
    }
    else
    {
        static const std::vector<AudioSound*> empty;
        return empty;
    }
}

AsyncTask::DoneStatus Audio3DManager::update(AsyncTask* task)
{
    if (!audio_manager_->get_active())
        return AsyncTask::DoneStatus::DS_cont;

    for (auto&& sound_dict_pair : sound_dict_)
    {
        auto& known_object = sound_dict_pair.first;

        auto node_path = known_object.get_node_path();
        if (!node_path)
        {
            // The node has been deleted.
            sound_dict_.erase(known_object);
            continue;
        }

        auto& sounds = sound_dict_pair.second;
        auto pos = node_path.get_pos(root_);

        for (auto&& sound : sounds)
        {
            auto vel = get_sound_velocity(sound);
            sound->set_3d_attributes(pos[0], pos[1], pos[2], vel[0], vel[1], vel[2]);
        }
    }

    // Update the position of the listener based on the object
    // to which it is attached
    if (listener_target_)
    {
        auto pos = listener_target_.get_pos(root_);
        auto forward = root_.get_relative_vector(listener_target_, LVector3::forward());
        auto up = root_.get_relative_vector(listener_target_, LVector3::up());
        auto vel = get_listener_velocity();
        audio_manager_->audio_3d_set_listener_attributes(pos[0], pos[1], pos[2], vel[0], vel[1], vel[2], forward[0], forward[1], forward[2], up[0], up[1], up[2]);
    }
    else
    {
        audio_manager_->audio_3d_set_listener_attributes(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1);
    }

    return AsyncTask::DoneStatus::DS_cont;
}

void Audio3DManager::disable()
{
    ShowBase::get_global_ptr()->get_task_mgr()->remove("Audio3DManager-updateTask");
    detach_listener();
    sound_dict_.clear();
}

}
