#pragma once

#include <boost/any.hpp>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/stage_manager.h>

namespace YAML {
class Node;
}

namespace rpcore {

// ************************************************************************************************
/** This is the base setting type, all setting types derive from this. */
class BaseType: public RPObject
{
public:
	BaseType(YAML::Node& data);
	virtual ~BaseType(void) {}

	virtual std::string get_value_as_string(void) const = 0;
	const boost::any& get_value(void) const { return _value; }

	virtual void set_value(const YAML::Node& value) = 0;

	virtual void add_defines(const std::string& plugin_id,
		const std::string& setting_id, StageManager::DefinesType& defines) const;

	const std::string& get_type(void) const { return _type; }
	const std::string& get_label(void) const { return _label; }
	const std::string& get_description(void) const { return _description; }
	bool is_runtime(void) const { return _runtime; }
	bool is_shader_runtime(void) const { return _shader_runtime; }

protected:
	boost::any _value;
	std::string _type;
	std::string _label;
	std::string _description;
	bool _runtime;
	bool _shader_runtime;
    std::unordered_map<std::string, std::string> _display_conditions;
};

// ************************************************************************************************
/**
 * This setting stores a single type including a minimum and maximum value.
 * It is shared between integer and floating point types.
 */
template <class T>
class TemplatedType: public BaseType
{
public:
	TemplatedType(YAML::Node& data);

	virtual std::string get_value_as_string(void) const override;
	virtual void set_value(const YAML::Node& value) override;
	virtual void set_value(T value);

protected:
	const std::string _template_type;
	T _default;
	T _minval;
	T _maxval;
};

template <class T>
TemplatedType<T>::TemplatedType(YAML::Node& data): BaseType(data), _template_type(typeid(T).name())
{
	_default = data["default"].as<T>();
	data.remove("default");
	_value = _default;

	const std::vector<T>& setting_range = data["range"].as<std::vector<T>>();
	_minval = setting_range[0];
	_maxval = setting_range[1];
	data.remove("range");
}

template <class T>
std::string TemplatedType<T>::get_value_as_string(void) const
{
	return std::to_string(boost::any_cast<T>(_value));
}

template <class T>
void TemplatedType<T>::set_value(const YAML::Node& value)
{
	set_value(value.as<T>());
}

template <class T>
void TemplatedType<T>::set_value(T value)
{
	if (_minval <= value && value <= _maxval)
		_value = value;
	else
		error(std::string("Invalid value: ") + std::to_string(value));
}

// ************************************************************************************************
/** Template instantiation of TemplatedType using int. */
using IntType = TemplatedType<int>;

/** Template instantiation of TemplatedType using float. */
using FloatType = TemplatedType<float>;

// ************************************************************************************************
/** Type for any power of two resolution. */
class PowerOfTwoType: public IntType
{
public:
    PowerOfTwoType(YAML::Node& data);

    virtual void set_value(int value) override;
};

inline PowerOfTwoType::PowerOfTwoType(YAML::Node& data): IntType(data)
{
}

inline void PowerOfTwoType::set_value(int value)
{
    if (_minval <= value && value <= _maxval)
    {
        // check if value is power of two.
        if (value && !(value & (value - 1)))
            _value = value;
        else
            error("Not a power of two: " + std::to_string(value));
    }
    else
    {
        error("Invalid value: " + std::to_string(value));
    }
}

// ************************************************************************************************
/** Boolean setting type. */
class BoolType: public BaseType
{
public:
	BoolType(YAML::Node& data);

	virtual std::string get_value_as_string(void) const;
	virtual void set_value(const YAML::Node& value);
	void set_value(bool value) { _value = value; }

private:
	bool _default;
};

// ************************************************************************************************
/** Enumeration setting type. */
class EnumType: public BaseType
{
public:
	EnumType(YAML::Node& data);

	virtual std::string get_value_as_string(void) const;
	virtual void set_value(const YAML::Node& value);
	void set_value(const std::string& value);
	virtual void add_defines(const std::string& plugin_id,
		const std::string& setting_id, StageManager::DefinesType& defines) const;

private:
	std::vector<std::string> _values;
	std::string _default;
};

// ************************************************************************************************
/** Type for any 2D or 3D sample sequence. */
class SampleSequenceType: public BaseType
{
public:
    static const std::vector<int> POISSON_2D_SIZES;
    static const std::vector<int> POISSON_3D_SIZES;
    static const std::vector<int> HALTON_SIZES;

    SampleSequenceType(YAML::Node& data);

    virtual std::string get_value_as_string(void) const override;
    virtual void set_value(const YAML::Node& value) override;
    void set_value(const std::string& value);

    std::vector<std::string> get_sequences(void) const;

private:
    int dimension_;
    std::string default_;
};

inline std::string SampleSequenceType::get_value_as_string(void) const
{
    return boost::any_cast<const std::string&>(_value);
}

inline void SampleSequenceType::set_value(const std::string& value)
{
    const auto& sequences = get_sequences();
    if (std::find(sequences.begin(), sequences.end(), value) == sequences.end())
    {
        error("Value '" + value + "' is not a valid sequence!");
        return;
    }
    _value = value;
}

// ************************************************************************************************
/** Path type to specify paths to files. */
class PathType: public BaseType
{
public:
	PathType(YAML::Node& data);

	virtual std::string get_value_as_string(void) const;
	virtual void set_value(const YAML::Node& value);
	void set_value(const std::string& value);
	virtual void add_defines(const std::string& plugin_id,
		const std::string& setting_id, StageManager::DefinesType& defines) const;

private:
	std::string _default;
	std::string _file_type;
	std::string _base_path;
};

inline std::string PathType::get_value_as_string(void) const
{
	return boost::any_cast<const std::string&>(_value);
}

inline void PathType::set_value(const std::string& value)
{
	_value = value;
}

// ************************************************************************************************
/**
 * Constructs a new setting from a given dataset. This method will automatically
 * instantiate a new class matching the type of the given dataset. It will fill
 * all values provided by the dataset and then return the created instance.
 */
std::shared_ptr<BaseType> make_setting_from_data(YAML::Node& data);

}	// namespace rpcore
