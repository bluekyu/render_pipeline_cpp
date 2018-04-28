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

#include "rpcore/pluginbase/setting_types.hpp"

#include <typeindex>
#include <sstream>

#include <boost/algorithm/string.hpp>

namespace rpcore {

enum class BaseTypeIndex: int
{
    IntType,
    FloatType,
    BoolType,
    EnumType,
    PathType,
    PowerOfTwoType,
    SampleSequenceType,
};

static const std::unordered_map<std::string, BaseTypeIndex> factory ={
    {"int", BaseTypeIndex::IntType},
    {"float", BaseTypeIndex::FloatType},
    {"bool", BaseTypeIndex::BoolType},
    {"enum", BaseTypeIndex::EnumType},
    {"path", BaseTypeIndex::PathType},
    {"power_of_two", BaseTypeIndex::PowerOfTwoType},
    {"sample_sequence", BaseTypeIndex::SampleSequenceType},
};

/**
 * Constructs a new setting from a given dataset, alongst with a factory
 * to resolve the setting type to.
 */
static std::shared_ptr<BaseType> make_setting_from_factory(YAML::Node& data)
{
    std::shared_ptr<BaseType> instance = nullptr;
    try
    {
        switch (factory.at(data["type"].as<std::string>()))
        {
        case BaseTypeIndex::IntType:
            instance = std::make_shared<IntType>(data);
            break;
        case BaseTypeIndex::FloatType:
            instance = std::make_shared<FloatType>(data);
            break;
        case BaseTypeIndex::BoolType:
            instance = std::make_shared<BoolType>(data);
            break;
        case BaseTypeIndex::EnumType:
            instance = std::make_shared<EnumType>(data);
            break;
        case BaseTypeIndex::PathType:
            instance = std::make_shared<PathType>(data);
            break;
        case BaseTypeIndex::PowerOfTwoType:
            instance = std::make_shared<PowerOfTwoType>(data);
            break;
        case BaseTypeIndex::SampleSequenceType:
            instance = std::make_shared<SampleSequenceType>(data);
            break;
        }
    }
    catch (const std::out_of_range&)
    {
        std::ostringstream s;
        s << data;
        throw std::out_of_range(std::string("Unknown setting type: ") + s.str());
    }

    return instance;
}

std::shared_ptr<BaseType> make_setting_from_data(YAML::Node& data)
{
    return make_setting_from_factory(data);
}

// ************************************************************************************************
BaseType::BaseType(YAML::Node& data): RPObject("BaseType")
{
    _type = data["type"].as<std::string>();
    data.remove("type");

    _label = boost::trim_copy(data["label"].as<std::string>());
    data.remove("label");

    _description = boost::trim_copy(data["description"].as<std::string>());
    data.remove("description");

    if (data["runtime"])
    {
        _runtime = data["runtime"].as<bool>();
        data.remove("runtime");
    }
    else
    {
        _runtime = false;
    }

    if (data["shader_runtime"])
    {
        _shader_runtime = data["shader_runtime"].as<bool>();
        data.remove("shader_runtime");
    }
    else
    {
        _shader_runtime = false;
    }

    for (auto key_val: data["display_if"])
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        _display_conditions.insert_or_assign(key_val.first.as<std::string>(), key_val.second.as<std::string>());
#else
        _display_conditions[key_val.first.as<std::string>()] = key_val.second.as<std::string>();
#endif
    data.remove("display_if");

    set_debug_name("psetting:" + _label);
}

void BaseType::add_defines(const std::string& plugin_id, const std::string& setting_id,
    StageManager::DefinesType& defines) const
{
    defines[plugin_id + "_" + setting_id] = get_value_as_string();
}

// ************************************************************************************************
BoolType::BoolType(YAML::Node& data): BaseType(data)
{
    _default = data["default"].as<bool>();
    data.remove("default");

    _value = _default;
}

std::string BoolType::get_value_as_string() const
{
    return boost::any_cast<bool>(_value) ? "1" : "0";
}

void BoolType::set_value(const YAML::Node& value)
{
    const std::string& b = boost::to_lower_copy(value.as<std::string>());
    if (b == "true" || b == "1")
        _value = true;
    else
        _value = false;
}

// ************************************************************************************************
EnumType::EnumType(YAML::Node& data): BaseType(data)
{
    _values = data["values"].as<std::vector<std::string>>();
    data.remove("values");

    _default = data["default"].as<std::string>();
    data.remove("default");

    if (std::find(_values.begin(), _values.end(), _default) == _values.end())
        throw std::runtime_error(std::string("Enum default not in enum values: ") + _default);

    _value = _default;
}

std::string EnumType::get_value_as_string() const
{
    return boost::any_cast<const std::string&>(_value);
}

void EnumType::set_value(const YAML::Node& value)
{
    set_value(value.as<std::string>());
}

void EnumType::set_value(const std::string& value)
{
    if (std::find(_values.begin(), _values.end(), value) == _values.end())
    {
        error("Value '" + value + "' not in enum values!");
        return;
    }
    _value = value;
}

void EnumType::add_defines(const std::string& plugin_id,
    const std::string& setting_id, StageManager::DefinesType& defines) const
{
    auto index = std::distance(_values.begin(), std::find(_values.begin(), _values.end(), boost::any_cast<const std::string&>(_value)));
    defines[plugin_id + "_" + setting_id] = std::to_string(1000 + index);

    for (size_t i=0, i_end=_values.size(); i < i_end; ++i)
        defines[std::string("enum_") + plugin_id + "_" + setting_id + "_" + _values[i]] = std::to_string(1000 + i);
}

// ************************************************************************************************
const std::vector<int> SampleSequenceType::POISSON_2D_SIZES = {4, 8, 12, 16, 32, 64};
const std::vector<int> SampleSequenceType::POISSON_3D_SIZES = {16, 32, 64};
const std::vector<int> SampleSequenceType::HALTON_SIZES = {4, 8, 16, 32, 64, 128};

SampleSequenceType::SampleSequenceType(YAML::Node& data): BaseType(data)
{
    dimension_ = data["dimension"].as<int>();
    data.remove("dimension");

    if (dimension_ != 2 && dimension_ != 3)
        throw std::runtime_error("Not a valid dimension, must be 2 or 3!");

    default_ = data["default"].as<std::string>();
    data.remove("default");

    const auto& sequences = get_sequences();
    if (std::find(sequences.begin(), sequences.end(), default_) == sequences.end())
        throw std::runtime_error("Not a valid sequence: " + default_);

    _value = default_;
}

void SampleSequenceType::set_value(const YAML::Node& value)
{
    set_value(value.as<std::string>());
}

std::vector<std::string> SampleSequenceType::get_sequences() const
{
    std::vector<std::string> result;
    if (dimension_ == 2)
    {
        for (const auto& dim: POISSON_2D_SIZES)
            result.push_back("poisson_2D_" + std::to_string(dim));
    }
    else
    {
        for (const auto& dim: POISSON_3D_SIZES)
            result.push_back("poisson_3D_" + std::to_string(dim));
    }

    for (int dim: HALTON_SIZES)
        result.push_back("halton_" + std::to_string(dimension_) + "D_" + std::to_string(dim));

    return result;
}

// ************************************************************************************************
PathType::PathType(YAML::Node& data): BaseType(data)
{
    _default = data["default"].as<std::string>("");
    data.remove("default");

    _value = _default;

    _file_type = data["file_type"].as<std::string>("");
    data.remove("file_type");

    _base_path = data["base_path"].as<std::string>("");
    data.remove("base_path");
}

void PathType::set_value(const YAML::Node& value)
{
    _value = value.as<std::string>();
}

void PathType::add_defines(const std::string& plugin_id,
    const std::string& setting_id, StageManager::DefinesType& defines) const
{
    // Paths are not available in shaders
}

}
