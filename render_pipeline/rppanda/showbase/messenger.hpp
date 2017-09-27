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

#pragma once

#include <throw_event.h>

#include <unordered_map>
#include <functional>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

class DirectObject;

class RENDER_PIPELINE_DECL Messenger
{
public:
    using EventName = std::string;
    using EventFunction = std::function<void(const Event*)>;

public:
    static Messenger* get_global_instance();

    Messenger();
    ~Messenger();

    size_t get_num_listners() const;
    size_t get_num_listners(const EventName& event_name) const;

    bool is_empty() const;

    void accept(const EventName& event_name, const EventFunction& method, DirectObject* object=nullptr,
        bool persistent=true);

    void ignore(const EventName& event_name, DirectObject* object = nullptr);
    void ignore_all(DirectObject* object = nullptr);

    void send(const EventName& event_name, bool queuing = true);
    void send(const EventName& event_name, const EventParameter& p1, bool queuing = true);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, bool queuing = true);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, const EventParameter& p3, bool queuing = true);

    void clear();

protected:
    using AcceptorType = std::pair<EventFunction, bool>;

    struct CallbackData
    {
        size_t size() const;
        bool empty() const;

        std::vector<AcceptorType> callbacks;
        std::unordered_map<DirectObject*, AcceptorType> object_callbacks;
    };

private:
    static void process_event(const Event* ev, void* user_data);

    void add_hook(const EventName& event_name);
    void remove_hook(const EventName& event_name);

    EventHandler* handler_;
    std::unordered_map<EventName, CallbackData> hooks_;
};

// ************************************************************************************************

inline Messenger::Messenger() : handler_(EventHandler::get_global_event_handler())
{
}

inline Messenger::~Messenger()
{
    clear();
}

inline size_t Messenger::get_num_listners() const
{
    size_t count = 0;
    for (const auto& hook: hooks_)
        count += hook.second.size();
    return count;
}

inline size_t Messenger::get_num_listners(const EventName& event_name) const
{
    auto found = hooks_.find(event_name);
    if (found != hooks_.end())
        return found->second.size();
    else
        return 0;
}

inline bool Messenger::is_empty() const
{
    return get_num_listners() == 0;
}

inline void Messenger::accept(const EventName& event_name, const EventFunction& method,
    DirectObject* object, bool persistent)
{
    add_hook(event_name);

    if (object)
    {
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        hooks_[event_name].object_callbacks.insert_or_assign(object, AcceptorType{ method, persistent });
#else
        hooks_[event_name].object_callbacks.insert({ object, AcceptorType{ method, persistent }});
#endif
    }
    else
    {
        hooks_[event_name].callbacks.push_back(AcceptorType{ method, persistent });
    }
}

inline void Messenger::ignore(const EventName& event_name, DirectObject* object)
{
    auto found = hooks_.find(event_name);
    if (found == hooks_.end())
        return;

    if (object)
    {
        auto& object_callbacks = found->second.object_callbacks;
        auto object_callbacks_found = object_callbacks.find(object);
        if (object_callbacks_found != object_callbacks.end())
            object_callbacks.erase(object_callbacks_found);
    }
    else
    {
        if (found != hooks_.end())
            found->second.callbacks.clear();
    }

    if (found->second.empty())
        remove_hook(event_name);
}

inline void Messenger::ignore_all(DirectObject* object)
{
    for (auto& hook : hooks_)
    {
        if (object)
        {
            auto& object_callbacks = hook.second.object_callbacks;
            auto found = object_callbacks.find(object);
            if (found != object_callbacks.end())
                object_callbacks.erase(found);
        }
        else
        {
            hook.second.callbacks.clear();
        }

        if (hook.second.empty())
            remove_hook(hook.first);
    }
}

inline void Messenger::send(const EventName& event_name, bool queuing)
{
    if (queuing)
        throw_event(event_name);
    else
        throw_event_directly(*handler_, event_name);
}

inline void Messenger::send(const EventName& event_name, const EventParameter& p1,
    bool queuing)
{
    if (queuing)
        throw_event(event_name, p1);
    else
        throw_event_directly(*handler_, event_name, p1);
}

inline void Messenger::send(const EventName& event_name, const EventParameter& p1,
    const EventParameter& p2, bool queuing)
{
    if (queuing)
        throw_event(event_name, p1, p2);
    else
        throw_event_directly(*handler_, event_name, p1, p2);
}

inline void Messenger::send(const EventName& event_name, const EventParameter& p1,
    const EventParameter& p2, const EventParameter& p3, bool queuing)
{
    if (queuing)
        throw_event(event_name, p1, p2, p3);
    else
        throw_event_directly(*handler_, event_name, p1, p2, p3);
}

inline size_t Messenger::CallbackData::size() const
{
    return callbacks.size() + object_callbacks.size();
}

inline bool Messenger::CallbackData::empty() const
{
    return callbacks.empty() && object_callbacks.empty();
}

inline void Messenger::add_hook(const EventName& event_name)
{
    if (!handler_->has_hook(event_name, &Messenger::process_event, this))
        handler_->add_hook(event_name, &Messenger::process_event, this);
}

inline void Messenger::remove_hook(const EventName& event_name)
{
    handler_->remove_hook(event_name, process_event, this);
    hooks_.erase(event_name);
}

inline void Messenger::clear()
{
    for (auto& hook : hooks_)
        handler_->remove_hook(hook.first, process_event, this);
    hooks_.clear();
}

}
