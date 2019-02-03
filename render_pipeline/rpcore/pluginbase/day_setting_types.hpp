/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
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

#include <luse.h>

#include <array>
#include <vector>
#include <string>
#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>

namespace YAML {
class Node;
}

namespace rpcore {

class SmoothConnectedCurve;

/** DayBase setting type for all setting types. */
class DayBaseType : public RPObject
{
public:
    using ValueType = std::pair<LVecBase3, int>;

public:
    DayBaseType(YAML::Node& data, const std::string& id="DayBaseType");
    virtual ~DayBaseType();

    virtual const std::string& get_glsl_type() const = 0;

    const std::string& get_type() const { return _type; }
    const std::string& get_label() const { return _label; }
    const std::string& get_description() const { return _description; }

    /** Returns the unscaled value at the given day time offset. */
    ValueType get_value_at(PN_stdfloat offset) const;

    /** Returns the scaled value at a given day time offset. */
    ValueType get_scaled_value_at(PN_stdfloat offset) const;

    /** Returns the scaled value from a given normalized value. */
    virtual ValueType get_scaled_value(const ValueType& values) const = 0;

    /** Returns the appropriate value for shader input. */
    virtual ValueType get_shader_input_value(PN_stdfloat offset) const = 0;

    /** Sets the control points on the curves. */
    void set_control_points(const std::vector<std::vector<LVecBase2>>& control_points);

    /** Serializes the setting to a yaml string. */
    std::string serialize() const;

protected:
    std::string _type;
    std::string _label;
    std::string _description;

    std::array<SmoothConnectedCurve*, 3> _curves;
};

/** Setting type storing a single scalar. */
class ScalarType : public DayBaseType
{
public:
    ScalarType(YAML::Node& data);

    virtual const std::string& get_glsl_type() const { return GLSL_TYPE; }

    /** Scales a linear value. */
    ValueType get_scaled_value(const ValueType& values) const final;

    ValueType get_shader_input_value(PN_stdfloat offset) const final;

    /** Linearizes a scaled value. */
    PN_stdfloat get_linear_value(PN_stdfloat scaled_value);

private:
    static const std::string GLSL_TYPE;

    std::string _unit;
    PN_stdfloat _minvalue;
    PN_stdfloat _maxvalue;
    PN_stdfloat _logarithmic_factor;
    PN_stdfloat _default;
};

inline DayBaseType::ValueType ScalarType::get_shader_input_value(PN_stdfloat offset) const
{
    return get_scaled_value_at(offset);
}

/** Setting type storing a RGB color triple. */
class ColorType : public DayBaseType
{
public:
    ColorType(YAML::Node& data);

    virtual const std::string& get_glsl_type() const { return GLSL_TYPE; }

    ValueType get_scaled_value(const ValueType& values) const final;

    ValueType get_shader_input_value(PN_stdfloat offset) const final;

    std::vector<PN_stdfloat> get_linear_value(const std::vector<PN_stdfloat>& scaled_value);

private:
    static const std::string GLSL_TYPE;

    std::vector<PN_stdfloat> _default;
};

inline DayBaseType::ValueType ColorType::get_shader_input_value(PN_stdfloat offset) const
{
    return get_value_at(offset);
}

/**
 * Constructs a new setting from a given dataset. This method will automatically
 * instantiate a new class matching the type of the given dataset. It will fill
 * all values provided by the dataset and then return the created instance.
 */
std::shared_ptr<DayBaseType> make_daysetting_from_data(YAML::Node& data);

}
