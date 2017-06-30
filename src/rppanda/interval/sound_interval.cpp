/**
 * This is C++ porting codes of direct/src/interval/SoundInterval.py
 */

#include "render_pipeline/rppanda/interval/sound_interval.hpp"

#include <audioSound.h>

#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/showbase/sfx_player.hpp"

namespace rppanda {

struct SoundInterval::Impl
{
    Impl(SoundInterval& self, const Parameters& params);

    void priv_initialize(double t);
    void priv_step(double t);
    void priv_finalize(void);

    SoundInterval& self_;

    std::string id_;
    PT(AudioSound) sound_;
    float sound_duration_;
    bool f_loop;
    double volume_;
    double start_time_;
    boost::optional<NodePath> node_;
    boost::optional<NodePath> listener_node_;
    boost::optional<float> cut_off_;
    bool seamless_loop_;
    bool sound_playing_;
    bool reverse_;

    bool in_finish_ = false;
};

SoundInterval::Impl::Impl(SoundInterval& self, const Parameters& params): self_(self)
{
    // Generate unique name
    id_ = "Sound-" + std::to_string(SoundInterval::sound_num_);
    SoundInterval::sound_num_ += 1;

    // Record instance variables
    sound_ = params.sound;

    if (sound_)
        sound_duration_ = sound_->length();
    else
        sound_duration_ = 0;

    f_loop = params.loop;
    volume_ = params.volume;
    start_time_ = params.start_time;
    node_ = params.node;
    listener_node_ = params.listener_node;
    cut_off_ = params.cut_off;
    seamless_loop_ = params.seamless_loop;
    if (seamless_loop_)
        f_loop = true;
    sound_playing_ = false;
    reverse_ = false;

    // If no duration given use sound's duration as interval's duration
    double duration = params.duration;
    if (duration == 0.0 && sound_)
        duration = (std::max)(sound_duration_ - start_time_, 0.0);
    self_._duration = duration;

    // Generate unique name if necessary
    if (params.name)
        self_._name = params.name.get();
    else
        self_._name = id_;
}

void SoundInterval::Impl::priv_initialize(double t)
{
    // If it's within a 10th of a second of the start,
    // start at the beginning.
    reverse_ = false;
    double t1 = t + start_time_;
    if (t1 < 0.1)
        t1 = 0.0;
    
    if (t1 < sound_duration_ && !(seamless_loop_ && sound_playing_))
    {
        ShowBase::get_global_ptr()->get_sfx_player()->play_sfx(
            sound_, f_loop, true, volume_, t1, node_, listener_node_, cut_off_);
        sound_playing_ = true;
    }

    self_._state = State::S_started;
    self_._curr_t = t;
}

void SoundInterval::Impl::priv_step(double t)
{
    if (self_._state == State::S_paused)
    {
        // Restarting from a pause.
        double t1 = t + start_time_;
        if (t1 < sound_duration_)
        {
            ShowBase::get_global_ptr()->get_sfx_player()->play_sfx(
                sound_, f_loop, true, volume_, t1, node_, listener_node_);
        }
    }

    if (listener_node_ && !listener_node_.get().is_empty() &&
        node_ && !node_.get().is_empty())
    {
        ShowBase::get_global_ptr()->get_sfx_player()->set_final_volume(
            sound_, node_, volume_, listener_node_, cut_off_);
    }

    self_._state = State::S_started;
    self_._curr_t = t;
}

void SoundInterval::Impl::priv_finalize(void)
{
    // if we're just coming to the end of a seamlessloop, leave the sound alone,
    // let the audio subsystem loop it
    if (seamless_loop_ && sound_playing_ && f_loop && in_finish_)
    {
        ShowBase::get_global_ptr()->get_sfx_player()->set_final_volume(
            sound_, node_, volume_, listener_node_, cut_off_);
        return;
    }
    else if (sound_)
    {
        sound_->stop();
        sound_playing_ = false;
    }

    self_._curr_t = self_.get_duration();
    self_._state = State::S_final;
}

// ************************************************************************************************
size_t SoundInterval::sound_num_ = 1;

TypeHandle SoundInterval::_type_handle;

SoundInterval::SoundInterval(const Parameters& params): CInterval("unnamed", 0.0f, false), impl_(std::make_unique<Impl>(*this, params))
{
}

SoundInterval::~SoundInterval(void) = default;

void SoundInterval::loop(double start_t, double end_t, double play_rate, bool stagger)
{
    impl_->f_loop = true;
    CInterval::loop(start_t, end_t, play_rate);
    if (stagger)
        set_t(double(std::rand()) / double(RAND_MAX) * get_duration());
}

void SoundInterval::finish(void)
{
    impl_->in_finish_ = true;
    CInterval::finish();
    impl_->in_finish_ = false;
}

void SoundInterval::priv_initialize(double t)
{
    impl_->priv_initialize(t);
}

void SoundInterval::priv_instant(void)
{
    return;
}

void SoundInterval::priv_step(double t)
{
    impl_->priv_step(t);
}

void SoundInterval::priv_finalize(void)
{
    impl_->priv_finalize();
}

void SoundInterval::priv_reverse_initialize(double t)
{
    impl_->reverse_ = true;
}

void SoundInterval::priv_reverse_instant(void)
{
    _state = State::S_initial;
}

void SoundInterval::priv_reverse_finalize(void)
{
    impl_->reverse_ = false;
    _state = State::S_initial;
}

void SoundInterval::priv_interrupt(void)
{
    if (impl_->sound_)
    {
        impl_->sound_->stop();
        impl_->sound_playing_ = false;
    }
    _state = State::S_paused;
}

}
