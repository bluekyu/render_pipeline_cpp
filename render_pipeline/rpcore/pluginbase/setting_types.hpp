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

#include <boost/any.hpp>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>

namespace YAML {
class Node;
}

namespace rpcore {

// ************************************************************************************************
/** This is the base setting type, all setting types derive from this. */
class RENDER_PIPELINE_DECL BaseType : public RPObject
{
public:
    BaseType(YAML::Node& data);
    virtual ~BaseType() = default;

    virtual std::string get_value_as_string() const = 0;
    const boost::any& get_default() const { return default_; }

    virtual void set_value(const YAML::Node& value) = 0;
    virtual void set_value(const boost::any& value) = 0;

    virtual void add_defines(const std::string& plugin_id,
        const std::string& setting_id, StageManager::DefinesType& defines) const;

    const std::string& get_type() const { return _type; }
    const std::string& get_label() const { return _label; }
    const std::string& get_description() const { return _description; }
    bool is_runtime() const { return _runtime; }
    bool is_shader_runtime() const { return _shader_runtime; }

    const std::unordered_map<std::string, std::string>& get_display_conditions() const;

    virtual void* downcast() = 0;
    virtual const void* downcast() const = 0;

protected:
    boost::any default_;
    boost::any _value;
    std::string _type;
    std::string _label;
    std::string _description;
    bool _runtime;
    bool _shader_runtime;
    std::unordered_map<std::string, std::string> display_conditions_;
};

inline const std::unordered_map<std::string, std::string>& BaseType::get_display_conditions() const
{
    return display_conditions_;
}

// ************************************************************************************************
/**
 * This setting stores a single type including a minimum and maximum value.
 * It is shared between integer and floating point types.
 */
template <class T>
class TemplatedType : public BaseType
{
public:
    using ValueType = T;

public:
    TemplatedType(YAML::Node& data);

    ValueType get_value_as_type() const;
    ValueType get_default_as_type() const;

    std::string get_value_as_string() const override;
    void set_value(const YAML::Node& value) override;
    void set_value(const boost::any& value) override;
    virtual void set_value(ValueType value);

    ValueType get_min() const;
    ValueType get_max() const;

    void* downcast() override { return this; }
    const void* downcast() const override { return this; }

protected:
    ValueType _minval;
    ValueType _maxval;
};

template <class T>
auto TemplatedType<T>::get_value_as_type() const -> ValueType
{
    return boost::any_cast<T>(_value);
}

template <class T>
auto TemplatedType<T>::get_default_as_type() const -> ValueType
{
    return boost::any_cast<T>(default_);
}

template <class T>
std::string TemplatedType<T>::get_value_as_string() const
{
    return std::to_string(get_value_as_type());
}

template <class T>
void TemplatedType<T>::set_value(const boost::any& value)
{
    set_value(boost::any_cast<T>(value));
}

template <class T>
void TemplatedType<T>::set_value(ValueType value)
{
    if (_minval <= value && value <= _maxval)
        _value = value;
    else
        error(std::string("Invalid value: ") + std::to_string(value));
}

template <class T>
auto TemplatedType<T>::get_min() const -> ValueType
{
    return _minval;
}

template <class T>
auto TemplatedType<T>::get_max() const -> ValueType
{
    return _maxval;
}

// ************************************************************************************************
/** Template instantiation of TemplatedType using int. */
using IntType = TemplatedType<int>;
template <> RENDER_PIPELINE_DECL TemplatedType<int>::TemplatedType(YAML::Node& data);
template <> RENDER_PIPELINE_DECL void TemplatedType<int>::set_value(const YAML::Node& value);

/** Template instantiation of TemplatedType using float. */
using FloatType = TemplatedType<float>;
template <> RENDER_PIPELINE_DECL TemplatedType<float>::TemplatedType(YAML::Node& data);
template <> RENDER_PIPELINE_DECL void TemplatedType<float>::set_value(const YAML::Node& value);

// ************************************************************************************************
/** Type for any power of two resolution. */
class RENDER_PIPELINE_DECL PowerOfTwoType : public IntType
{
public:
    PowerOfTwoType(YAML::Node& data);

    void set_value(ValueType value) final;
};

inline PowerOfTwoType::PowerOfTwoType(YAML::Node& data): IntType(data)
{
}

inline void PowerOfTwoType::set_value(ValueType value)
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
class RENDER_PIPELINE_DECL BoolType : public BaseType
{
public:
    using ValueType = bool;

public:
    BoolType(YAML::Node& data);

    ValueType get_value_as_type() const;
    ValueType get_default_as_type() const;

    std::string get_value_as_string() const final;
    void set_value(const YAML::Node& value) final;

    /** Accept bool or std::string */
    void set_value(const boost::any& value) final;

    void set_value(ValueType value) { _value = value; }

    void* downcast() override { return this; }
    const void* downcast() const override { return this; }

private:
    void set_value(const std::string& value);
};

inline auto BoolType::get_value_as_type() const -> ValueType
{
    return boost::any_cast<bool>(_value);
}

inline auto BoolType::get_default_as_type() const -> ValueType
{
    return boost::any_cast<bool>(default_);
}

inline void BoolType::set_value(const boost::any& value)
{
    if (auto v = boost::any_cast<bool>(&value))
        _value = *v;
    else if (auto v = boost::any_cast<std::string>(&value))
        set_value(*v);
}

// ************************************************************************************************
/** Enumeration setting type. */
class RENDER_PIPELINE_DECL EnumType : public BaseType
{
public:
    using ValueType = std::string;

public:
    EnumType(YAML::Node& data);

    const ValueType& get_value_as_type() const;
    const ValueType& get_default_as_type() const;

    std::string get_value_as_string() const final;

    void set_value(const YAML::Node& value) final;
    void set_value(const boost::any& value) final;
    void set_value(const ValueType& value);

    void add_defines(const std::string& plugin_id,
        const std::string& setting_id, StageManager::DefinesType& defines) const final;

    void* downcast() override { return this; }
    const void* downcast() const override { return this; }

private:
    std::vector<ValueType> _values;
};

inline auto EnumType::get_value_as_type() const -> const ValueType&
{
    return *boost::any_cast<std::string>(&_value);
}

inline auto EnumType::get_default_as_type() const -> const ValueType&
{
    return *boost::any_cast<std::string>(&default_);
}

inline std::string EnumType::get_value_as_string() const
{
    return get_value_as_type();
}

inline void EnumType::set_value(const boost::any& value)
{
    set_value(boost::any_cast<std::string>(value));
}

// ************************************************************************************************
/** Type for any 2D or 3D sample sequence. */
class RENDER_PIPELINE_DECL SampleSequenceType : public BaseType
{
public:
    using ValueType = std::string;

public:
    static const std::vector<int> POISSON_2D_SIZES;
    static const std::vector<int> POISSON_3D_SIZES;
    static const std::vector<int> HALTON_SIZES;

    SampleSequenceType(YAML::Node& data);

    const ValueType& get_value_as_type() const;
    const ValueType& get_default_as_type() const;

    std::string get_value_as_string() const final;
    void set_value(const YAML::Node& value) final;
    void set_value(const boost::any& value) final;
    void set_value(const ValueType& value);

    std::vector<ValueType> get_sequences() const;

    void* downcast() override { return this; }
    const void* downcast() const override { return this; }

private:
    int dimension_;
};

inline auto SampleSequenceType::get_value_as_type() const -> const ValueType&
{
    return *boost::any_cast<ValueType>(&_value);
}

inline auto SampleSequenceType::get_default_as_type() const -> const ValueType&
{
    return *boost::any_cast<ValueType>(&default_);
}

inline std::string SampleSequenceType::get_value_as_string() const
{
    return get_value_as_type();
}

inline void SampleSequenceType::set_value(const boost::any& value)
{
    set_value(boost::any_cast<ValueType>(value));
}

inline void SampleSequenceType::set_value(const ValueType& value)
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
class RENDER_PIPELINE_DECL PathType : public BaseType
{
public:
    using ValueType = std::string;

public:
    PathType(YAML::Node& data);

    const ValueType& get_value_as_type() const;
    const ValueType& get_default_as_type() const;

    std::string get_value_as_string() const final;

    void set_value(const YAML::Node& value) final;
    void set_value(const boost::any& value) final;
    void set_value(const ValueType& value);

    void add_defines(const std::string& plugin_id,
        const std::string& setting_id, StageManager::DefinesType& defines) const final;

    void* downcast() override { return this; }
    const void* downcast() const override { return this; }

private:
    ValueType _default;
    ValueType _file_type;
    ValueType _base_path;
};

inline auto PathType::get_value_as_type() const -> const ValueType&
{
    return *boost::any_cast<ValueType>(&_value);
}

inline auto PathType::get_default_as_type() const -> const ValueType&
{
    return *boost::any_cast<ValueType>(&default_);
}

inline std::string PathType::get_value_as_string() const
{
    return get_value_as_type();
}

inline void PathType::set_value(const boost::any& value)
{
    set_value(boost::any_cast<std::string>(value));
}

inline void PathType::set_value(const ValueType& value)
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

}
