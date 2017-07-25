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

class RENDER_PIPELINE_DECL SoundInterval: public CInterval
{
public:
    static size_t sound_num_;   //!< Name counter

public:
    SoundInterval(AudioSound* sound, bool loop=false, double duration=0.0,
        const boost::optional<std::string>& name={}, double volume=1.0, double start_time=0.0,
        boost::optional<NodePath> node={}, bool seamless_loop=true, boost::optional<NodePath> listener_node={},
        boost::optional<float> cut_off={});

    ~SoundInterval(void) = default;

    AudioSound* get_sound(void) const;
    float get_sound_duration(void) const;
    bool get_f_loop(void) const;
    double get_volume(void) const;
    double get_start_time(void) const;
    boost::optional<NodePath> get_node(void) const;
    boost::optional<NodePath> get_listener_node(void) const;
    boost::optional<float> get_cut_off(void) const;

    void loop(double start_t=0.0, double end_t=-1.0, double play_rate=1.0, bool stagger=false);
    void finish(void);

    void priv_initialize(double t) override;
    void priv_instant(void) override;
    void priv_step(double t) override;
    void priv_finalize(void) override;
    void priv_reverse_initialize(double t) override;
    void priv_reverse_instant(void) override;
    void priv_reverse_finalize(void) override;
    void priv_interrupt(void) override;

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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline AudioSound* SoundInterval::get_sound(void) const
{
    return sound_;
}

inline float SoundInterval::get_sound_duration(void) const
{
    return sound_duration_;
}

inline bool SoundInterval::get_f_loop(void) const
{
    return f_loop_;
}

inline double SoundInterval::get_volume(void) const
{
    return volume_;
}

inline double SoundInterval::get_start_time(void) const
{
    return start_time_;
}

inline boost::optional<NodePath> SoundInterval::get_node(void) const
{
    return node_;
}

inline boost::optional<NodePath> SoundInterval::get_listener_node(void) const
{
    return listener_node_;
}

inline boost::optional<float> SoundInterval::get_cut_off(void) const
{
    return cut_off_;
}

inline TypeHandle SoundInterval::get_class_type(void)
{
    return type_handle_;
}

inline void SoundInterval::init_type(void)
{
    CInterval::init_type();
    register_type(type_handle_, "rppanda::SoundInterval", CInterval::get_class_type());
}

inline TypeHandle SoundInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle SoundInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
