/**
 * This is C++ porting codes of direct/src/interval/MetaInterval.py
 */

#include "render_pipeline/rppanda/interval/meta_interval.hpp"

#include "rppanda/interval/config_rppanda_interval.hpp"

namespace rppanda {

int MetaInterval::sequence_num_ = 1;

TypeHandle MetaInterval::type_handle_;
TypeHandle Sequence::type_handle_;
TypeHandle Parallel::type_handle_;
TypeHandle ParallelEndTogether::type_handle_;
TypeHandle Track::type_handle_;

MetaInterval::MetaInterval(std::initializer_list<CInterval*> ivals, const Parameters& params):
    MetaInterval(params.name ? params.name.get() : ("MetaInterval" + std::to_string(sequence_num_++)), params)
{
    if (ivals.size() == 0)
        ;
    else if (ivals.size() == 1)
        add_c_interval(*ivals.begin(), 0.0, CMetaInterval::RS_level_begin);
    else
        rppanda_interval_cat.error() << "Cannot build list from MetaInterval directly." << std::endl;
}

}
