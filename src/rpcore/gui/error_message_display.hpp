#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.hpp>

class LineStream;

namespace rpcore {

class ErrorMessageDisplay: public RPObject
{
public:
    ErrorMessageDisplay(void);
    ~ErrorMessageDisplay(void);

    /** Updates the error display, fetching all new messages from the notify stream. */
    void update(void);

    /** Adds a new error message. */
    void add_error(const std::string& msg);

    /** Adds a new warning message. */
    void add_warning(const std::string& msg);

    /** Internal method to add a new text to the output. */
    void add_text(const std::string& text, const LVecBase3f& color);

    /** Clears all messages / removes them. */
    void clear_messages(void);

    void show(void);

    void hide(void);

private:
    /** Internal method to init the stream to catch all notify messages. */
    void init_notify(void);

    int _num_errors = 0;
    NodePath _error_node;
    LineStream* _notify_stream = nullptr;
};

// ************************************************************************************************
inline void ErrorMessageDisplay::show(void)
{
    _error_node.show();
}

inline void ErrorMessageDisplay::hide(void)
{
    _error_node.hide();
}

}
