#pragma once

#include <nodePath.h>

#include <functional>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rppanda {
class DirectCheckBox;
}

namespace rpcore {

/**
 * This is a wrapper around DirectCheckBox, providing a simpler interface
 * and better visuals.
 */
class RENDER_PIPELINE_DECL Checkbox: public RPObject
{
public:
    struct Parameters
    {
        NodePath parent = NodePath();
        float x = 0;
        float y = 0;
        std::function<void(bool, void*)> callback;
        void* extra_args = nullptr;
        bool radio = false;
        int expand_width = 100;
        bool checked = false;
        bool enabled = true;
    };

public:
    Checkbox(const Parameters& params=Parameters());

    /** Returns whether the node is currently checked. */
    bool is_checked(void) const;

    /** Returns a handle to the internally used node. */
    rppanda::DirectCheckBox* get_node(void) const;

    /** Internal method when another checkbox in the same radio group changed it's value. */
    void update_status(void* args);

    /** Internal method to check/uncheck the checkbox. */
    void set_checked(bool val, bool do_callback=true);

private:
    PT(rppanda::DirectCheckBox) _node;

    std::function<void(bool, void*)> _callback;
    void* _extra_args = nullptr;
};

inline rppanda::DirectCheckBox* Checkbox::get_node(void) const
{
    return _node;
}

}
