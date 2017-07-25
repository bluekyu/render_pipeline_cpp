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
    ActorInterval(Actor* actor, const std::string& anim_name, bool loop=false, bool constrained_loop=false,
        boost::optional<double> duration={}
    );

private:
    WPT(Actor) actor_;

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline TypeHandle ActorInterval::get_class_type(void)
{
    return type_handle_;
}

inline void ActorInterval::init_type(void)
{
    CInterval::init_type();
    register_type(type_handle_, "rppanda::ActorInterval", CInterval::get_class_type());
}

inline TypeHandle ActorInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle ActorInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
