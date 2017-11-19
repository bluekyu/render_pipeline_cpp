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

/**
 * Wrapper of Panda3D EventHandler.
 *
 * This is used to accept std::function as hook or hook with DirectObject.
 * Messenger is NOT thread-safe except send() functions with queuing.
 */
class RENDER_PIPELINE_DECL Messenger : public TypedObject
{
public:
    using EventName = std::string;
    using EventFunction = std::function<void(const Event*)>;

    using AcceptorType = std::pair<EventFunction, bool>;
    using AcceptorList = std::list<AcceptorType>;
    using AcceptorMap = std::unordered_map<DirectObject*, AcceptorType>;

    struct CallbackData
    {
        size_t size() const;
        bool empty() const;

        AcceptorList callbacks;
        AcceptorMap object_callbacks;
    };

public:
    static Messenger* get_global_instance();

    Messenger();
    ~Messenger();

    size_t get_num_listners() const;
    size_t get_num_listners(const EventName& event_name) const;

    bool is_empty() const;

    /**
     * Add the method to hook with given event name.
     *
     * @return Iterator of callbacks.
     */
    AcceptorList::const_iterator accept(const EventName& event_name, const EventFunction& method, bool persistent=true);
    AcceptorMap::const_iterator accept(const EventName& event_name, const EventFunction& method, DirectObject* object,
        bool persistent = true);

    /** Ignore the event has @event_name. */
    void ignore(const EventName& event_name, const AcceptorList::const_iterator& iter);

    /** Ignore the event has @event_name binding to DirectObject. */
    void ignore(const EventName& event_name, const AcceptorMap::const_iterator& object);

    /** Ignore the event has @event_name binding to DirectObject. */
    void ignore(const EventName& event_name, DirectObject* object);

    /**
     * Ignore all events has @event_name.
     *
     * If @include_object is false, only events not binded to DirectObject will be ignored.
     */
    void ignore_all(const EventName& event_name, bool include_object=true);

    /** Ignore all events binding to DirectObject. */
    void ignore_all(DirectObject* object);

    void send(const EventName& event_name, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, const EventParameter& p3, bool queuing = false);

    void clear();

private:
    static void process_event(const Event* ev, void* user_data);

    void add_hook(const EventName& event_name);
    void remove_hook(const EventName& event_name);

    EventHandler* handler_;
    std::unordered_map<EventName, CallbackData> hooks_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
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
    const auto found = hooks_.find(event_name);
    if (found != hooks_.end())
        return found->second.size();
    else
        return 0;
}

inline bool Messenger::is_empty() const
{
    return get_num_listners() == 0;
}

inline auto Messenger::accept(const EventName& event_name, const EventFunction& method,
    bool persistent) -> AcceptorList::const_iterator
{
    add_hook(event_name);
    hooks_[event_name].callbacks.push_back(AcceptorType{ method, persistent });
    return --hooks_[event_name].callbacks.end();
}

inline void Messenger::ignore(const EventName& event_name, const AcceptorList::const_iterator& iter)
{
    auto found = hooks_.find(event_name);
    if (found == hooks_.end())
        return;

    found->second.callbacks.erase(iter);

    if (found->second.empty())
        remove_hook(event_name);
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
        found->second.callbacks.clear();
    }

    if (found->second.empty())
        remove_hook(event_name);
}

inline void Messenger::ignore_all(const EventName& event_name, bool include_object)
{
    auto found = hooks_.find(event_name);
    if (found == hooks_.end())
        return;

    found->second.callbacks.clear();
    if (include_object)
        found->second.object_callbacks.clear();

    if (found->second.empty())
        remove_hook(event_name);
}

inline void Messenger::ignore_all(DirectObject* object)
{
    auto iter = hooks_.begin();
    const auto iter_end = hooks_.end();
    while (iter != iter_end)
    {
        if (object)
        {
            auto& object_callbacks = iter->second.object_callbacks;
            auto found = object_callbacks.find(object);
            if (found != object_callbacks.end())
                object_callbacks.erase(found);
        }
        else
        {
            iter->second.callbacks.clear();
        }

        // iterator becomes invalidated after erase.
        auto iter_tmp = iter;
        ++iter;
        if (iter_tmp->second.empty())
            remove_hook(iter_tmp->first);
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
    // NOTE: std::list::size has O(1) in C++11
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
    for (auto&& hook : hooks_)
        handler_->remove_hook(hook.first, process_event, this);
    hooks_.clear();
}

inline TypeHandle Messenger::get_class_type()
{
    return type_handle_;
}

inline void Messenger::init_type()
{
    TypedObject::init_type();
    register_type(type_handle_, "rppanda::Messenger", TypedObject::get_class_type());
}

inline TypeHandle Messenger::get_type() const
{
    return get_class_type();
}

inline TypeHandle Messenger::force_init_type()
{
    init_type();
    return get_class_type();
}

}
