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
 * This module defines the various "meta intervals", which execute other
 * intervals either in parallel or in a specified sequential order.
 *
 * This is C++ porting codes of direct/src/interval/MetaInterval.py
 */

#pragma once

#include <cMetaInterval.h>

#include <tuple>
#include <initializer_list>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL MetaInterval : public CMetaInterval
{
protected:
    static int sequence_num_;

public:
    MetaInterval(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name={},
        bool auto_pause=false, bool auto_finish=false);

protected:
    MetaInterval(const std::string& name, bool auto_pause, bool auto_finish);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

inline MetaInterval::MetaInterval(const std::string& name, bool auto_pause, bool auto_finish): CMetaInterval(name)
{
    set_auto_pause(auto_pause);
    set_auto_finish(auto_finish);
}

inline TypeHandle MetaInterval::get_class_type()
{
    return type_handle_;
}

inline void MetaInterval::init_type()
{
    CMetaInterval::init_type();
    register_type(type_handle_, "rppanda::MetaInterval", CMetaInterval::get_class_type());
}

inline TypeHandle MetaInterval::get_type() const
{
    return get_class_type();
}

inline TypeHandle MetaInterval::force_init_type()
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Sequence : public MetaInterval
{
public:
    Sequence(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name={},
        bool auto_pause=false, bool auto_finish=false, double duration=-1);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

inline Sequence::Sequence(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name,
    bool auto_pause, bool auto_finish, double duration):
    MetaInterval(name ? name.get() : ("Sequence" + std::to_string(sequence_num_++)), auto_pause, auto_finish)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, 0.0, CMetaInterval::RS_previous_end);
    pop_level(duration);
}

inline TypeHandle Sequence::get_class_type()
{
    return type_handle_;
}

inline void Sequence::init_type()
{
    MetaInterval::init_type();
    register_type(type_handle_, "rppanda::Sequence", MetaInterval::get_class_type());
}

inline TypeHandle Sequence::get_type() const
{
    return get_class_type();
}

inline TypeHandle Sequence::force_init_type()
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Parallel : public MetaInterval
{
public:
    Parallel(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name={},
        bool auto_pause=false, bool auto_finish=false, double duration=-1);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

inline Parallel::Parallel(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name,
    bool auto_pause, bool auto_finish, double duration):
    MetaInterval(name ? name.get() : ("Parallel" + std::to_string(sequence_num_++)), auto_pause, auto_finish)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, 0.0, CMetaInterval::RS_level_begin);
    pop_level(duration);
}

inline TypeHandle Parallel::get_class_type()
{
    return type_handle_;
}

inline void Parallel::init_type()
{
    MetaInterval::init_type();
    register_type(type_handle_, "rppanda::Parallel", MetaInterval::get_class_type());
}

inline TypeHandle Parallel::get_type() const
{
    return get_class_type();
}

inline TypeHandle Parallel::force_init_type()
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL ParallelEndTogether : public MetaInterval
{
public:
    ParallelEndTogether(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name={},
        bool auto_pause=false, bool auto_finish=false, double duration=-1);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

inline ParallelEndTogether::ParallelEndTogether(std::initializer_list<CInterval*> ivals,
    const boost::optional<std::string>& name, bool auto_pause, bool auto_finish, double duration):
    MetaInterval(name ? name.get() : ("ParallelEndTogether" + std::to_string(sequence_num_++)), auto_pause, auto_finish)
{
    double max_duration = 0;
    for (CInterval* ival: ivals)
        max_duration = (std::max)(max_duration, ival->get_duration());

    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, max_duration - ival->get_duration(), CMetaInterval::RS_level_begin);
    pop_level(duration);
}

inline TypeHandle ParallelEndTogether::get_class_type()
{
    return type_handle_;
}

inline void ParallelEndTogether::init_type()
{
    MetaInterval::init_type();
    register_type(type_handle_, "rppanda::ParallelEndTogether", MetaInterval::get_class_type());
}

inline TypeHandle ParallelEndTogether::get_type() const
{
    return get_class_type();
}

inline TypeHandle ParallelEndTogether::force_init_type()
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Track : public MetaInterval
{
public:
    using TrackType = std::tuple<double, CInterval*, RelativeStart>;

    Track(std::initializer_list<TrackType> track_list, const boost::optional<std::string>& name={},
        bool auto_pause=false, bool auto_finish=false, double duration=-1);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

inline Track::Track(std::initializer_list<TrackType> track_list, const boost::optional<std::string>& name,
    bool auto_pause, bool auto_finish, double duration):
    MetaInterval(name ? name.get() : ("Track" + std::to_string(sequence_num_++)), auto_pause, auto_finish)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (const auto& track: track_list)
        add_c_interval(std::get<1>(track), std::get<0>(track), std::get<2>(track));
    pop_level(duration);
}

inline TypeHandle Track::get_class_type()
{
    return type_handle_;
}

inline void Track::init_type()
{
    MetaInterval::init_type();
    register_type(type_handle_, "rppanda::Track", MetaInterval::get_class_type());
}

inline TypeHandle Track::get_type() const
{
    return get_class_type();
}

inline TypeHandle Track::force_init_type()
{
    init_type();
    return get_class_type();
}

}
