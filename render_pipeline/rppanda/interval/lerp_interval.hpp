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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpNodePathInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpNodePathInterval::init_type(void)
{
    CLerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpNodePathInterval", CLerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpNodePathInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpNodePathInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpPosInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpPosInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpPosInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpPosInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpPosInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpHprInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpHprInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpHprInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpHprInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpHprInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpQuatInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpQuatInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpQuatInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpQuatInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpQuatInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpScaleInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpScaleInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpScaleInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpScaleInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpScaleInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpShearInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpShearInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpShearInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpShearInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpShearInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
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
    static TypeHandle get_class_type(void);
    static void init_type(void);
    virtual TypeHandle get_type(void) const;
    virtual TypeHandle force_init_type(void);

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

inline TypeHandle LerpPosHprInterval::get_class_type(void)
{
    return type_handle_;
}

inline void LerpPosHprInterval::init_type(void)
{
    LerpNodePathInterval::init_type();
    register_type(type_handle_, "rppanda::LerpPosHprInterval", LerpNodePathInterval::get_class_type());
}

inline TypeHandle LerpPosHprInterval::get_type(void) const
{
    return get_class_type();
}

inline TypeHandle LerpPosHprInterval::force_init_type(void)
{
    init_type();
    return get_class_type();
}

}
