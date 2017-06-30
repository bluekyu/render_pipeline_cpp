/**
 * This is C++ porting codes of direct/src/interval/MetaInterval.py
 */

#include "render_pipeline/rppanda/interval/meta_interval.hpp"

#include "rppanda/config_rppanda.hpp"

namespace rppanda {

int MetaInterval::sequence_num_ = 1;

TypeHandle MetaInterval::_type_handle;
TypeHandle Sequence::_type_handle;
TypeHandle Parallel::_type_handle;
TypeHandle ParallelEndTogether::_type_handle;
TypeHandle Track::_type_handle;

MetaInterval::MetaInterval(std::initializer_list<CInterval*> ivals, const Parameters& params):
	MetaInterval(params.name ? params.name.get() : ("MetaInterval" + std::to_string(sequence_num_++)), params)
{
	if (ivals.size() == 0)
		;
	else if (ivals.size() == 1)
		add_c_interval(*ivals.begin(), 0.0, CMetaInterval::RS_level_begin);
	else
		rppanda_cat.error() << "Cannot build list from MetaInterval directly." << std::endl;
}

}
