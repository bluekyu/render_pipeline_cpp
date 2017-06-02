#pragma once

#include <cLerpNodePathInterval.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.hpp>

namespace rppanda {

/**
 * This is the base class for all of the lerps, defined below, that
 * affect a property on a NodePath, like pos or hpr.
 */
class RENDER_PIPELINE_DECL LerpNodePathInterval: public CLerpNodePathInterval
{
protected:
	static int lerp_node_path_num;

public:
	struct Parameters
	{
		NodePath nodepath;
		boost::optional<std::string> name;
		double duration;
		BlendType blend_type = BlendType::BT_no_blend;
		bool bake_in_start = true;
		bool fluid = false;
		NodePath other = NodePath();
	};

public:
	LerpNodePathInterval(const Parameters& params): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
	}

protected:
	LerpNodePathInterval(const std::string& name, const Parameters& params): CLerpNodePathInterval(
		name,
		params.duration, params.blend_type,
		params.bake_in_start, params.fluid, params.nodepath, params.other)
	{
		++lerp_node_path_num;
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		CLerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpNodePathInterval", CLerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpPosInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		LVecBase3 pos;
		boost::optional<LVecBase3> start_pos;
	};

public:
	LerpPosInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" +std::to_string(lerp_node_path_num)),
		params)
	{
		set_end_pos(params.pos);
		if (params.start_pos)
			set_start_pos(params.start_pos.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpPosInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpHprInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		LVecBase3 hpr;
		boost::optional<LVecBase3> start_hpr;
		boost::optional<LQuaternion> start_quat;
	};

public:
	LerpHprInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
		set_end_hpr(params.hpr);
		if (params.start_hpr)
			set_start_hpr(params.start_hpr.get());
		if (params.start_quat)
			set_start_quat(params.start_quat.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpHprInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpQuatInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		boost::optional<LQuaternion> quat;
		boost::optional<LVecBase3> start_hpr;
		boost::optional<LQuaternion> start_quat;
		boost::optional<LVecBase3> hpr;
	};

public:
	LerpQuatInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
		LQuaternion quat;
		if (params.quat)
		{
			quat = params.quat.get();
		}
		else
		{
			if (!params.hpr)
				throw std::runtime_error("HPR value does not exist!");
			quat.set_hpr(params.hpr.get());
		}

		set_end_quat(quat);
		if (params.start_hpr)
			set_start_hpr(params.start_hpr.get());
		if (params.start_quat)
			set_start_quat(params.start_quat.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpQuatInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpScaleInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		LVecBase3 scale;
		boost::optional<LVecBase3> start_scale;
	};

public:
	LerpScaleInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
		set_end_scale(params.scale);
		if (params.start_scale)
			set_start_hpr(params.start_scale.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpScaleInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpShearInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		LVecBase3 shear;
		boost::optional<LVecBase3> start_shear;
	};

public:
	LerpShearInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
		set_end_scale(params.shear);
		if (params.start_shear)
			set_start_hpr(params.start_shear.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpShearInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpPosHprInterval: public LerpNodePathInterval
{
public:
	struct Parameters: public LerpNodePathInterval::Parameters
	{
		LVecBase3 pos;
		LVecBase3 hpr;
		boost::optional<LVecBase3> start_pos;
		boost::optional<LVecBase3> start_hpr;
		boost::optional<LQuaternion> start_quat;
	};

public:
	LerpPosHprInterval(const Parameters& params=Parameters()): LerpNodePathInterval(
		params.name ? params.name.get() :
			(get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
		params)
	{
		set_end_pos(params.pos);
		if (params.start_pos)
			set_start_pos(params.start_pos.get());
		set_end_hpr(params.hpr);
		if (params.start_hpr)
			set_start_hpr(params.start_hpr.get());
		if (params.start_quat)
			set_start_quat(params.start_quat.get());
	}

public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		LerpNodePathInterval::init_type();
		register_type(_type_handle, "LerpPosHprInterval", LerpNodePathInterval::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
	static TypeHandle _type_handle;
};

}
