#pragma once

#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpcore {

class RenderPipeline;

class RENDER_PIPELINE_DECL DayTimeManager: public RPObject
{
public:
    DayTimeManager(RenderPipeline& pipeline);
    ~DayTimeManager(void);

    /**
     * Returns the current time of day as floating point number
     * from 0 to 1, whereas 0 means 00:00 and 1 means 24:00 (=00:00)
     */
    float get_time(void) const;

    /**
     * Sets the current time of day as floating point number from
     * 0 to 1, whereas 0 means 00:00 and 1 means 24:00 (=00:00). Any
     * number greater than 1 will be reduced to fit the 0 .. 1 range by
     * doing time modulo 1.
     */
    void set_time(float time);

    /**
     * Sets the current time of day as floating point number from
     * 0 to 1, whereas 0 means 00:00 and 1 means 24:00 (=00:00).
     * A string in the format 'hh:mm' can be passed.
     */
    void set_time(const std::string& time);

    /** Returns the current time as formatted string, e.g. 12:34. */
    std::string get_formatted_time(void) const;

    /**
     * Loads all day time settings from the plugin manager and registers
     * them to the used input buffer.
     */
    void load_settings(void);

    /** Internal update method which updates all day time settings. */
    void update(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
