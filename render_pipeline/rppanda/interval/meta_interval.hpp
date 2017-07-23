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

class RENDER_PIPELINE_DECL MetaInterval: public CMetaInterval
{
protected:
    static int sequence_num_;

public:
    struct Parameters
    {
        boost::optional<std::string> name;
        bool auto_pause = false;
        bool auto_finish = false;
        double duration = -1;
    };

public:
    MetaInterval(std::initializer_list<CInterval*> ivals, const Parameters& params=Parameters());

protected:
    MetaInterval(const std::string& name, const Parameters& params=Parameters());

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

inline MetaInterval::MetaInterval(const std::string& name, const Parameters& params): CMetaInterval(name)
{
    set_auto_pause(params.auto_pause);
    set_auto_finish(params.auto_finish);
}

inline TypeHandle MetaInterval::get_class_type(void)
{
    return type_handle_;
}

inline void MetaInterval::init_type(void)
{
    CMetaInterval::init_type();
    register_type(type_handle_, "MetaInterval", CMetaInterval::get_class_type());
}

inline TypeHandle MetaInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle MetaInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Sequence: public MetaInterval
{
public:
    Sequence(std::initializer_list<CInterval*> ivals, const Parameters& params=Parameters());

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

inline Sequence::Sequence(std::initializer_list<CInterval*> ivals, const Parameters& params):
    MetaInterval(params.name ? params.name.get() : ("Sequence" + std::to_string(sequence_num_++)), params)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, 0.0, CMetaInterval::RS_previous_end);
    pop_level(params.duration);
}

inline TypeHandle Sequence::get_class_type(void)
{
    return type_handle_;
}

inline void Sequence::init_type(void)
{
    MetaInterval::init_type();
    register_type(type_handle_, "Sequence", MetaInterval::get_class_type());
}

inline TypeHandle Sequence::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle Sequence::force_init_type(void)
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Parallel: public MetaInterval
{
public:
    Parallel(std::initializer_list<CInterval*> ivals, const Parameters& params=Parameters());

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

inline Parallel::Parallel(std::initializer_list<CInterval*> ivals, const Parameters& params):
    MetaInterval(params.name ? params.name.get() : ("Parallel" + std::to_string(sequence_num_++)), params)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, 0.0, CMetaInterval::RS_level_begin);
    pop_level(params.duration);
}

inline TypeHandle Parallel::get_class_type(void)
{
    return type_handle_;
}

inline void Parallel::init_type(void)
{
    MetaInterval::init_type();
    register_type(type_handle_, "Parallel", MetaInterval::get_class_type());
}

inline TypeHandle Parallel::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle Parallel::force_init_type(void)
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL ParallelEndTogether: public MetaInterval
{
public:
    ParallelEndTogether(std::initializer_list<CInterval*> ivals, const Parameters& params=Parameters());

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

inline ParallelEndTogether::ParallelEndTogether(std::initializer_list<CInterval*> ivals, const Parameters& params):
    MetaInterval(params.name ? params.name.get() : ("ParallelEndTogether" + std::to_string(sequence_num_++)), params)
{
    double max_duration = 0;
    for (CInterval* ival: ivals)
        max_duration = (std::max)(max_duration, ival->get_duration());

    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (CInterval* ival: ivals)
        add_c_interval(ival, max_duration - ival->get_duration(), CMetaInterval::RS_level_begin);
    pop_level(params.duration);
}

inline TypeHandle ParallelEndTogether::get_class_type(void)
{
    return type_handle_;
}

inline void ParallelEndTogether::init_type(void)
{
    MetaInterval::init_type();
    register_type(type_handle_, "ParallelEndTogether", MetaInterval::get_class_type());
}

inline TypeHandle ParallelEndTogether::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle ParallelEndTogether::force_init_type(void)
{
    init_type();
    return get_class_type();
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL Track: public MetaInterval
{
public:
    using TrackType = std::tuple<double, CInterval*, RelativeStart>;

    Track(std::initializer_list<TrackType> track_list, const Parameters& params=Parameters());

public:
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

private:
    static TypeHandle type_handle_;
};

inline Track::Track(std::initializer_list<TrackType> track_list, const Parameters& params):
    MetaInterval(params.name ? params.name.get() : ("Track" + std::to_string(sequence_num_++)), params)
{
    push_level(get_name(), 0, CMetaInterval::RS_level_begin);
    for (const auto& track: track_list)
        add_c_interval(std::get<1>(track), std::get<0>(track), std::get<2>(track));
    pop_level(params.duration);
}

inline TypeHandle Track::get_class_type(void)
{
    return type_handle_;
}

inline void Track::init_type(void)
{
    MetaInterval::init_type();
    register_type(type_handle_, "Track", MetaInterval::get_class_type());
}

inline TypeHandle Track::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle Track::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
