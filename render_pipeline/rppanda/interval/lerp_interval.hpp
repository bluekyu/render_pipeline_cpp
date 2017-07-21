/**
 * LerpInterval module: contains the LerpInterval class
 *
 * This is C++ porting codes of direct/src/interval/LerpInterval.py
 */

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
    LerpNodePathInterval(NodePath nodepath, const boost::optional<std::string>& name, double duration,
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true, bool fluid=false,
        NodePath other={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        CLerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpNodePathInterval", CLerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpNodePathInterval::LerpNodePathInterval(NodePath nodepath, const boost::optional<std::string>& name, double duration,
    BlendType blend_type, bool bake_in_start, bool fluid,
    NodePath other): CLerpNodePathInterval(
        name ? name.get() : (get_class_type().get_name() + "-" + std::to_string(lerp_node_path_num)),
        duration, blend_type, bake_in_start, fluid, nodepath, other)
{
    ++lerp_node_path_num;
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpPosInterval: public LerpNodePathInterval
{
public:
    LerpPosInterval(NodePath nodepath, double duration, const LVecBase3& pos,
        const boost::optional<LVecBase3>& start_pos={}, NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpPosInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpPosInterval::LerpPosInterval(NodePath nodepath, double duration, const LVecBase3& pos,
    const boost::optional<LVecBase3>& start_pos, NodePath other,
    BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name):
    LerpNodePathInterval(nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    set_end_pos(pos);
    if (start_pos)
        set_start_pos(start_pos.get());
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpHprInterval: public LerpNodePathInterval
{
public:
    LerpHprInterval(NodePath nodepath, double duration, const LVecBase3& hpr,
        const boost::optional<LVecBase3>& start_hpr={},
        const boost::optional<LQuaternion>& start_quat={}, NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpHprInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpHprInterval::LerpHprInterval(NodePath nodepath, double duration, const LVecBase3& hpr,
    const boost::optional<LVecBase3>& start_hpr,
    const boost::optional<LQuaternion>& start_quat, NodePath other,
    BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name): LerpNodePathInterval(
        nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    set_end_hpr(hpr);
    if (start_hpr)
        set_start_hpr(start_hpr.get());
    if (start_quat)
        set_start_quat(start_quat.get());
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpQuatInterval: public LerpNodePathInterval
{
public:
    LerpQuatInterval(NodePath nodepath, double duration,
        const boost::optional<LQuaternion>& quat={},
        const boost::optional<LVecBase3>& start_hpr={},
        const boost::optional<LQuaternion>& start_quat={},
        NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={},
        const boost::optional<LVecBase3>& hpr={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpQuatInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpQuatInterval::LerpQuatInterval(NodePath nodepath, double duration,
    const boost::optional<LQuaternion>& quat,
    const boost::optional<LVecBase3>& start_hpr,
    const boost::optional<LQuaternion>& start_quat,
    NodePath other,
    BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name,
    const boost::optional<LVecBase3>& hpr): LerpNodePathInterval(
        nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    LQuaternion quat__;
    if (quat)
    {
        quat__ = quat.get();
    }
    else
    {
        if (!hpr)
            throw std::runtime_error("HPR value does not exist!");
        quat__.set_hpr(hpr.get());
    }

    set_end_quat(quat__);
    if (start_hpr)
        set_start_hpr(start_hpr.get());
    if (start_quat)
        set_start_quat(start_quat.get());
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpScaleInterval: public LerpNodePathInterval
{
public:
    LerpScaleInterval(NodePath nodepath, double duration,
        const LVecBase3& scale,
        const boost::optional<LVecBase3>& start_scale={},
        NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpScaleInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpScaleInterval::LerpScaleInterval(NodePath nodepath, double duration,
    const LVecBase3& scale,
    const boost::optional<LVecBase3>& start_scale,
    NodePath other, BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name): LerpNodePathInterval(
        nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    set_end_scale(scale);
    if (start_scale)
        set_start_hpr(start_scale.get());
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpShearInterval: public LerpNodePathInterval
{
public:
    LerpShearInterval(NodePath nodepath, double duration,
        const LVecBase3& shear,
        const boost::optional<LVecBase3>& start_shear={},
        NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpShearInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpShearInterval::LerpShearInterval(NodePath nodepath, double duration,
    const LVecBase3& shear,
    const boost::optional<LVecBase3>& start_shear,
    NodePath other,
    BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name): LerpNodePathInterval(
        nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    set_end_scale(shear);
    if (start_shear)
        set_start_hpr(start_shear.get());
}

// ************************************************************************************************
class RENDER_PIPELINE_DECL LerpPosHprInterval: public LerpNodePathInterval
{
public:
    LerpPosHprInterval(NodePath nodepath, double duration,
        const LVecBase3& pos, const LVecBase3& hpr,
        const boost::optional<LVecBase3>& start_pos={},
        const boost::optional<LVecBase3>& start_hpr={},
        const boost::optional<LQuaternion>& start_quat={},
        NodePath other={},
        BlendType blend_type=BlendType::BT_no_blend, bool bake_in_start=true,
        bool fluid=false, const boost::optional<std::string>& name={});

public:
    static TypeHandle get_class_type()
    {
        return type_handle_;
    }
    static void init_type()
    {
        LerpNodePathInterval::init_type();
        register_type(type_handle_, "LerpPosHprInterval", LerpNodePathInterval::get_class_type());
    }
    virtual TypeHandle get_type() const
    {
        return get_class_type();
    }
    virtual TypeHandle force_init_type() { init_type(); return get_class_type(); }

private:
    static TypeHandle type_handle_;
};

inline LerpPosHprInterval::LerpPosHprInterval(NodePath nodepath, double duration,
    const LVecBase3& pos, const LVecBase3& hpr,
    const boost::optional<LVecBase3>& start_pos,
    const boost::optional<LVecBase3>& start_hpr,
    const boost::optional<LQuaternion>& start_quat,
    NodePath other,
    BlendType blend_type, bool bake_in_start,
    bool fluid, const boost::optional<std::string>& name): LerpNodePathInterval(
        nodepath, name, duration, blend_type, bake_in_start, fluid, other)
{
    set_end_pos(pos);
    if (start_pos)
        set_start_pos(start_pos.get());
    set_end_hpr(hpr);
    if (start_hpr)
        set_start_hpr(start_hpr.get());
    if (start_quat)
        set_start_quat(start_quat.get());
}

}
