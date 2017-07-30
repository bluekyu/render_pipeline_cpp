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
