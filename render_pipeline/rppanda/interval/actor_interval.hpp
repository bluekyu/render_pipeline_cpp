#pragma once

#include <cInterval.h>

#include <boost/optional.hpp>

namespace rppanda {

class Actor;

class ActorInterval: public CInterval
{
public:
    // Name counter
    static int anim_num_;

public:
    ActorInterval(const std::shared_ptr<Actor>& actor, const std::string& anim_name, bool loop=false, bool constrained_loop=false,
        boost::optional<double> duration={}
    );

private:
    std::shared_ptr<Actor> actor_;

public:
    static TypeHandle get_class_type(void)
    {
        return type_handle_;
    }
    static void init_type(void)
    {
        CInterval::init_type();
        register_type(type_handle_, "SoundInterval", CInterval::get_class_type());
    }
    virtual TypeHandle get_type(void) const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

}
