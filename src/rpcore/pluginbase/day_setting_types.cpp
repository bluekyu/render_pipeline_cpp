#include <render_pipeline/rpcore/pluginbase/day_setting_types.hpp>

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
	try
	{
		factory.at(data["type"].as<std::string>());
	}
	catch (const std::out_of_range&)
	{
		std::ostringstream s;
		s << data["type"];
		throw std::out_of_range(std::string("Unkown setting type: ") + s.str());
	}

	std::shared_ptr<DayBaseType> instance = nullptr;
	try
	{
		switch (factory.at(data["type"].as<std::string>()))
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
		std::cout << "Exception occured while parsing " << data;
		throw err;
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

DayBaseType::~DayBaseType(void)
{
	for (auto& c: _curves)
		delete c;
}

DayBaseType::ValueType DayBaseType::get_value_at(float offset) const
{
	if (_curves[1] == nullptr)
		return { LVecBase3f(_curves[0]->get_value(offset), 0, 0), 1 } ;

	LVecBase3f result(
		_curves[0]->get_value(offset),
		_curves[1]->get_value(offset),
		_curves[2]->get_value(offset));
	
	return { result, 3 };
}

DayBaseType::ValueType DayBaseType::get_scaled_value_at(float offset) const
{
	return get_scaled_value(get_value_at(offset));
}

void DayBaseType::set_control_points(const std::vector<std::vector<LVecBase2f>>& control_points)
{
	for (size_t index=0, index_end=control_points.size(); index < index_end; ++index)
		_curves[index]->set_control_points(control_points[index]);
}

std::string DayBaseType::serialize(void) const
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
const std::string ScalarType::glsl_type = "float";

ScalarType::ScalarType(YAML::Node& data): DayBaseType(data, "ScalarType")
{
	_unit = data["unit"].as<std::string>("none");
	data.remove("unit");

	const std::vector<float>& setting_range = data["range"].as<std::vector<float>>();
	_minvalue = setting_range[0];
	_maxvalue = setting_range[1];
	data.remove("range");

	_logarithmic_factor = data["logarithmic_factor"].as<float>(1.0f);

	static const std::vector<std::string> unit_list = { "degree", "meter", "percent", "klux", "none" };
	if (std::find(unit_list.begin(), unit_list.end(), _unit) == unit_list.end())
		throw std::runtime_error(std::string("Invalid unit type: ") + _unit);

	_default = get_linear_value(data["default"].as<float>());

	_curves[0] = new SmoothConnectedCurve;
	_curves[0]->set_single_value(_default);
}

ScalarType::ValueType ScalarType::get_scaled_value(const ValueType& values) const
{
	if (_logarithmic_factor != 1.0)
	{
		float exp_mult = std::exp(_logarithmic_factor * values.first[0] * 4.0f) - 1.0f;
		float exp_div = std::exp(_logarithmic_factor * 4.0f) - 1.0f;
		return { LVecBase3f(exp_mult / exp_div * (_maxvalue - _minvalue) + _minvalue, 0, 0), 1 };
	}
	else
	{
		return { LVecBase3f(values.first[0] * (_maxvalue - _minvalue) + _minvalue, 0, 0), 1 };
	}
}

float ScalarType::get_linear_value(float scaled_value)
{
	float result = (scaled_value - _minvalue) / (_maxvalue - _minvalue);
	if (_logarithmic_factor != 1.0)
	{
		result *= std::exp(_logarithmic_factor * 4.0f) - 1.0f;
		result = std::log(result + 1.0f) / (4.0f * _logarithmic_factor);
	}
	return result;
}

// ************************************************************************************************
const std::string ColorType::glsl_type = "vec3";

ColorType::ColorType(YAML::Node& data): DayBaseType(data, "ColorType")
{
	_default = get_linear_value(data["default"].as<std::vector<float>>());

	std::vector<LVecBase3f> colors = {
		LVecBase3f(255, 0, 0), LVecBase3f(0, 255, 0), LVecBase3f(0, 0, 255)
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
		LVecBase3f(
			values.first[0] * 255.0f,
			values.first[1] * 255.0f,
			values.first[2] * 255.0f),
		3,
	};
}

std::vector<float> ColorType::get_linear_value(const std::vector<float>& scaled_value)
{
	std::vector<float> v;
	for (float f: scaled_value)
		v.push_back(f * 255.0f);
	return v;
}

}
