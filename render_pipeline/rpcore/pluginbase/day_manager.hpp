/**
 * Render Pipeline C++
 *
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
