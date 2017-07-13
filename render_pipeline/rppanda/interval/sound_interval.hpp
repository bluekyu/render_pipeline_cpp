/**
 * SoundInterval module: contains the SoundInterval class.
 *
 * This is C++ porting codes of direct/src/interval/SoundInterval.py
 */

#pragma once

#include <nodePath.h>
#include <cInterval.h>

#include <boost/optional.hpp>

class AudioSound;

namespace rppanda {

class SoundInterval: public CInterval
{
public:
    static size_t sound_num_;   //!< Name counter

public:
    struct Parameters
    {
        AudioSound* sound = nullptr;
        bool loop = false;
        double duration = 0.0;
        boost::optional<std::string> name;
        double volume = 1.0;
        double start_time = 0.0;
        boost::optional<NodePath> node;
        bool seamless_loop = true;
        boost::optional<NodePath> listener_node;
        boost::optional<float> cut_off;
    };

public:
    SoundInterval(const Parameters& params);

    ~SoundInterval(void);

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

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

public:
    static TypeHandle get_class_type()
    {
        return _type_handle;
    }
    static void init_type()
    {
        CInterval::init_type();
        register_type(_type_handle, "SoundInterval", CInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle _type_handle;
};

}
