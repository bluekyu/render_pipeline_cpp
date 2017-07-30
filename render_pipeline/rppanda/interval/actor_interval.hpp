/**
 * Render Pipeline C++
 *
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
