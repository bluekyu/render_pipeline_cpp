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

#include "rppanda/showbase/config_rppanda_showbase.hpp"

namespace rppanda {

const Messenger::AcceptorMap Messenger::empty_acceptor_;
const Messenger::EventSetType Messenger::empty_events_;

Messenger::Messenger() : handler_(EventHandler::get_global_event_handler())
{
}

Messenger::~Messenger()
{
    clear();
}

Messenger* Messenger::get_global_instance()
{
    static Messenger instance;
    return &instance;
}

void Messenger::accept(const EventName& event_name, const EventFunction& method,
    const DirectObject* object, bool persistent)
{
    if (!object)
    {
        rppanda_showbase_cat.error() << "Messenger::accept is called with null DirectObject: " << event_name << std::endl;
        return;
    }

    add_hook(event_name);

    auto& object_callbacks = callbacks_[event_name];

    auto found = object_callbacks.find(object);
    if (found == object_callbacks.end())
    {
        object_callbacks.emplace(object, AcceptorType{ method, persistent });
    }
    else
    {
        if (rppanda_showbase_cat.is_debug())
            rppanda_showbase_cat.warning() << "object: Messenger accept: \"" << event_name << "\" new callback" << std::endl;

        found->second = AcceptorType{ method, persistent };
    }

    object_events_[object].insert(event_name);
}

auto Messenger::who_accepts(const EventName& event_name) const -> const AcceptorMap&
{
    auto found = callbacks_.find(event_name);
    if (found != callbacks_.end())
        return found->second;
    else
        return empty_acceptor_;
}

void Messenger::process_event(const Event* ev, void* user_data)
{
    const auto& event_name = ev->get_name();

    auto self = reinterpret_cast<Messenger*>(user_data);
    auto& acceptor_map = self->callbacks_.at(event_name);

    // call in object_callbacks
    {
        auto iter = acceptor_map.cbegin();
        auto iter_end = acceptor_map.cend();
        for (; iter != iter_end; ++iter)
        {
            iter->second.first(ev);

            // NOTE: the erased element is only invalidated, so we can use the iterator.
            if (!iter->second.second)
                acceptor_map.erase(iter);
        }
    }

    if (acceptor_map.empty())
        self->remove_hook(event_name);
}

}
