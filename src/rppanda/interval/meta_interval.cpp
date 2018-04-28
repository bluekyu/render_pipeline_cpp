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

MetaInterval::MetaInterval(std::initializer_list<CInterval*> ivals, const boost::optional<std::string>& name,
    bool auto_pause, bool auto_finish):
    MetaInterval(name ? name.value() : ("MetaInterval" + std::to_string(sequence_num_++)),
        auto_pause, auto_finish)
{
    if (ivals.size() == 0)
        ;
    else if (ivals.size() == 1)
        add_c_interval(*ivals.begin(), 0.0, CMetaInterval::RS_level_begin);
    else
        rppanda_interval_cat.error() << "Cannot build list from MetaInterval directly." << std::endl;
}

}
