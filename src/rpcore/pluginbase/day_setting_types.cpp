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

#include "render_pipeline/rpcore/pluginbase/day_setting_types.hpp"

#include <boost/algorithm/string.hpp>

#include "rplibs/yaml.hpp"

#include "rpcore/util/smooth_connected_curve.hpp"

namespace rpcore {

enum class DayBaseTypeIndex: int
{
    ColorType,
    ScalarType,
};

static const std::unordered_map<std::string, DayBaseTypeIndex> factory ={
    { "color", DayBaseTypeIndex::ColorType },
    { "scalar", DayBaseTypeIndex::ScalarType },
};

/**
 * Constructs a new setting from a given dataset, alongst with a factory
 * to resolve the setting type to.
 */
static std::shared_ptr<DayBaseType> make_setting_from_factory(YAML::Node& data)
{
    const auto& setting_type = data["type"].as<std::string>();
    const auto& found = factory.find(setting_type);
    if (found == factory.end())
    {
        std::ostringstream s;
        s << data["type"];
        throw std::runtime_error(std::string("Unknown setting type: ") + s.str());
    }

    std::shared_ptr<DayBaseType> instance = nullptr;
    try
    {
        switch (found->second)
        {
        case DayBaseTypeIndex::ColorType:
            instance = std::make_shared<ColorType>(data);
            break;
        case DayBaseTypeIndex::ScalarType:
            instance = std::make_shared<ScalarType>(data);
            break;
        }
    }
    catch (const std::exception& err)
    {
        RPObject::global_error("DaySettingTypes", std::string("Parsing error: ") + err.what());

        std::ostringstream s;
        s << data;
        throw std::runtime_error("Exception occured while parsing: " + s.str());
    }

    return instance;
}

std::shared_ptr<DayBaseType> make_daysetting_from_data(YAML::Node& data)
{
    return make_setting_from_factory(data);
}

// ************************************************************************************************
DayBaseType::DayBaseType(YAML::Node& data, const std::string& id): RPObject(id)
{
    _curves = { { nullptr, nullptr, nullptr } };

    _type = data["type"].as<std::string>();
    data.remove("type");

    _label = boost::trim_copy(data["label"].as<std::string>());
    data.remove("label");

    _description = boost::trim_copy(data["description"].as<std::string>());
    data.remove("description");

    set_debug_name("dsetting:" + _label);
}

DayBaseType::~DayBaseType()
{
    for (auto&& c: _curves)
        delete c;
}

DayBaseType::ValueType DayBaseType::get_value_at(PN_stdfloat offset) const
{
    if (_curves[1] == nullptr)
        return { LVecBase3(_curves[0]->get_value(offset), 0, 0), 1 } ;

    LVecBase3 result(
        _curves[0]->get_value(offset),
        _curves[1]->get_value(offset),
        _curves[2]->get_value(offset));

    return { result, 3 };
}

DayBaseType::ValueType DayBaseType::get_scaled_value_at(PN_stdfloat offset) const
{
    return get_scaled_value(get_value_at(offset));
}

void DayBaseType::set_control_points(const std::vector<std::vector<LVecBase2>>& control_points)
{
    for (size_t index=0, index_end=control_points.size(); index < index_end; ++index)
        _curves[index]->set_control_points(control_points[index]);
}

std::string DayBaseType::serialize() const
{
    std::string result = "[";
    for (const auto& curve: _curves)
    {
        result += curve->serialize();
        result += ",";
    }
    if (_curves.size() > 0)
        result.pop_back();
    result += "]";
    return result;
}

// ************************************************************************************************
const std::string ScalarType::GLSL_TYPE = "float";

ScalarType::ScalarType(YAML::Node& data): DayBaseType(data, "ScalarType")
{
    _unit = data["unit"].as<std::string>("none");
    data.remove("unit");

    const std::vector<PN_stdfloat>& setting_range = data["range"].as<std::vector<PN_stdfloat>>();
    _minvalue = setting_range[0];
    _maxvalue = setting_range[1];
    data.remove("range");

    _logarithmic_factor = data["logarithmic_factor"].as<PN_stdfloat>(1.0f);

    static const std::vector<std::string> unit_list = { "degree", "meter", "percent", "klux", "none" };
    if (std::find(unit_list.begin(), unit_list.end(), _unit) == unit_list.end())
        throw std::runtime_error(std::string("Invalid unit type: ") + _unit);

    _default = get_linear_value(data["default"].as<PN_stdfloat>());

    _curves[0] = new SmoothConnectedCurve;
    _curves[0]->set_single_value(_default);
}

ScalarType::ValueType ScalarType::get_scaled_value(const ValueType& values) const
{
    if (_logarithmic_factor != 1.0)
    {
        PN_stdfloat exp_mult = std::exp(_logarithmic_factor * values.first[0] * 4) - 1;
        PN_stdfloat exp_div = std::exp(_logarithmic_factor * 4) - 1;
        return { LVecBase3(exp_mult / exp_div * (_maxvalue - _minvalue) + _minvalue, 0, 0), 1 };
    }
    else
    {
        return { LVecBase3(values.first[0] * (_maxvalue - _minvalue) + _minvalue, 0, 0), 1 };
    }
}

PN_stdfloat ScalarType::get_linear_value(PN_stdfloat scaled_value)
{
    PN_stdfloat result = (scaled_value - _minvalue) / (_maxvalue - _minvalue);
    if (_logarithmic_factor != 1.0)
    {
        result *= std::exp(_logarithmic_factor * 4) - 1;
        result = std::log(result + 1) / (4 * _logarithmic_factor);
    }
    return result;
}

// ************************************************************************************************
const std::string ColorType::GLSL_TYPE = "vec3";

ColorType::ColorType(YAML::Node& data): DayBaseType(data, "ColorType")
{
    _default = get_linear_value(data["default"].as<std::vector<PN_stdfloat>>());

    std::vector<LVecBase3> colors = {
        LVecBase3(255, 0, 0), LVecBase3(0, 255, 0), LVecBase3(0, 0, 255)
    };

    for (int i = 0; i < 3; ++i)
    {
        SmoothConnectedCurve* curve = new SmoothConnectedCurve;
        curve->set_single_value(_default[i]);
        curve->set_color(colors[i]);
        _curves[i] = curve;
    }
}

ColorType::ValueType ColorType::get_scaled_value(const ValueType& values) const
{
    return {
        LVecBase3(
            values.first[0] * 255,
            values.first[1] * 255,
            values.first[2] * 255),
        3,
    };
}

std::vector<PN_stdfloat> ColorType::get_linear_value(const std::vector<PN_stdfloat>& scaled_value)
{
    std::vector<PN_stdfloat> v;
    for (PN_stdfloat f: scaled_value)
        v.push_back(f * 255);
    return v;
}

}
