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

#include <list>
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

    using AcceptorType = std::pair<EventFunction, bool>;    //  { function, persistent }
    using AcceptorMap = std::unordered_map<const DirectObject*, AcceptorType>;
    using CallbacksType = std::unordered_map<EventName, AcceptorMap>;

public:
    static Messenger* get_global_instance();

    Messenger();

    virtual ~Messenger();

    size_t get_num_listners() const;
    size_t get_num_listners(const EventName& event_name) const;

    AsyncFuture* get_future(const EventName& event_name) const;

    /**
     * Add the method to hook with given event name.
     *
     * @return Iterator of callbacks.
     */
    void accept(const EventName& event_name, const EventFunction& method, const DirectObject* object,
        bool persistent = true);

    /** Ignore the event has @event_name binding to DirectObject. */
    void ignore(const EventName& event_name, const DirectObject* object);

    /**
     * Ignore all events has @event_name.
     */
    void ignore_all(const EventName& event_name);

    /** Ignore all events binding to DirectObject. */
    void ignore_all(const DirectObject* object);

    /** Returns the list of all events accepted by the indicated object. */
    std::vector<EventName> get_all_accepting(const DirectObject* object) const;

    /** Is this object accepting this event? */
    bool is_accepting(const EventName& event_name, const DirectObject* object) const;

    /** Return objects accepting the given event. */
    const AcceptorMap& who_accepts(const EventName& event_name) const;

    /** Is this object ignoring this event? */
    bool is_ignoring(const EventName& event_name, const DirectObject* object) const;

    void send(const EventName& event_name, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, bool queuing = false);
    void send(const EventName& event_name, const EventParameter& p1,
        const EventParameter& p2, const EventParameter& p3, bool queuing = false);

    /** Start fresh with a clear dict. */
    void clear();

    bool is_empty() const;

    std::vector<EventName> get_events() const;

private:
    static void process_event(const Event* ev, void* user_data);

    /**
     * Add hook with @p event_name for Messenger::process_event
     *
     * If the hook already exists, then this does not add.
     */
    void add_hook(const EventName& event_name);

    /** Remove hook with @p event_name for Messenger::process_event */
    void remove_hook(const EventName& event_name);

    EventHandler* handler_;
    CallbacksType callbacks_;

    static const AcceptorMap empty_acceptor_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline size_t Messenger::get_num_listners() const
{
    size_t count = 0;
    for (const auto& callback: callbacks_)
        count += callback.second.size();
    return count;
}

inline size_t Messenger::get_num_listners(const EventName& event_name) const
{
    const auto found = callbacks_.find(event_name);
    if (found != callbacks_.end())
        return found->second.size();
    else
        return 0;
}

inline AsyncFuture* Messenger::get_future(const EventName& event_name) const
{
    return handler_->get_future(event_name);
}

inline void Messenger::ignore(const EventName& event_name, const DirectObject* object)
{
    auto found = callbacks_.find(event_name);
    if (found == callbacks_.end())
        return;

    auto& acceptor_map = found->second;
    auto acceptor_map_found = acceptor_map.find(object);
    if (acceptor_map_found != acceptor_map.end())
        acceptor_map.erase(acceptor_map_found);

    if (acceptor_map.empty())
        remove_hook(event_name);
}

inline void Messenger::ignore_all(const EventName& event_name)
{
    auto found = callbacks_.find(event_name);
    if (found == callbacks_.end())
        return;

    remove_hook(event_name);
}

inline void Messenger::ignore_all(const DirectObject* object)
{
    auto iter = callbacks_.begin();
    const auto iter_end = callbacks_.end();
    while (iter != iter_end)
    {
        auto& acceptor_map = iter->second;
        auto found = acceptor_map.find(object);
        if (found != acceptor_map.end())
            acceptor_map.erase(found);

        // iterator becomes invalidated after erase.
        auto iter_tmp = iter;
        ++iter;
        if (iter_tmp->second.empty())
            remove_hook(iter_tmp->first);
    }
}

inline auto Messenger::get_all_accepting(const DirectObject* object) const -> std::vector<EventName>
{
    std::vector<EventName> events;
    for (const auto& kv : callbacks_)
    {
        const auto& acceptor_map = kv.second;
        if (acceptor_map.find(object) != acceptor_map.end())
            events.push_back(kv.first);
    }
    return events;
}

inline bool Messenger::is_accepting(const EventName& event_name, const DirectObject* object) const
{
    auto found = callbacks_.find(event_name);
    if (found != callbacks_.end())
    {
        if (found->second.find(object) != found->second.end())
            return true;
    }

    return false;
}

inline bool Messenger::is_ignoring(const EventName& event_name, const DirectObject* object) const
{
    return !is_accepting(event_name, object);
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

inline void Messenger::add_hook(const EventName& event_name)
{
    if (!handler_->has_hook(event_name, &Messenger::process_event, this))
        handler_->add_hook(event_name, &Messenger::process_event, this);
}

inline void Messenger::remove_hook(const EventName& event_name)
{
    handler_->remove_hook(event_name, process_event, this);
    callbacks_.erase(event_name);
}

inline void Messenger::clear()
{
    for (auto&& kv : callbacks_)
        handler_->remove_hook(kv.first, process_event, this);
    callbacks_.clear();
}

inline bool Messenger::is_empty() const
{
    return get_num_listners() == 0;
}

inline auto Messenger::get_events() const -> std::vector<EventName>
{
    std::vector<EventName> events;
    events.reserve(callbacks_.size());
    for (const auto& kv: callbacks_)
        events.push_back(kv.first);
    return events;
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
