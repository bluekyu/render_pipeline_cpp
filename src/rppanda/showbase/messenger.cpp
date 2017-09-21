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
 * This is C++ porting codes of direct/src/showbase/Messenger.py
 */

#include "render_pipeline/rppanda/showbase/messenger.hpp"

namespace rppanda {

Messenger* Messenger::get_global_instance()
{
    static Messenger instance;
    return &instance;
}

void Messenger::process_event(const Event* ev, void* user_data)
{
    static std::vector<AcceptorType> callbacks_processing;

    const auto& event_name = ev->get_name();

    auto self = reinterpret_cast<Messenger*>(user_data);
    auto& hook = self->hooks_.at(event_name);

    // call in callbacks
    hook.callbacks.swap(callbacks_processing);
    hook.callbacks.reserve(callbacks_processing.size());

    for (size_t k = 0, k_end = callbacks_processing.size(); k < k_end; ++k)
    {
        callbacks_processing[k].first(ev);
        if (callbacks_processing[k].second)
            hook.callbacks.push_back(std::move(callbacks_processing[k]));
    }
    callbacks_processing.clear();

    // call in object_callbacks
    auto iter = hook.object_callbacks.begin();
    auto iter_end = hook.object_callbacks.end();
    for (; iter != iter_end; ++iter)
    {
        iter->second.first(ev);
        if (!iter->second.second)
            hook.object_callbacks.erase(iter);
    }

    if (hook.empty())
        self->remove_hook(event_name);
}

}
