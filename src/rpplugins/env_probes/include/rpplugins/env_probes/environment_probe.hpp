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

#include <render_pipeline/rpcore/rpobject.hpp>

#include <transformState.h>
#include <boundingSphere.h>

namespace rpplugins {

/** Simple class, representing an environment probe. */
class EnvironmentProbe : public rpcore::RPObject
{
public:
    EnvironmentProbe();

    int get_index() const { return _index; }
    void set_index(int index) { _index = index; }

    int get_last_update() const { return _last_update; }
    void set_last_update(int update) { _last_update = update; }

    BoundingSphere* get_bounds() const;

    /** Returns whether the probe was modified since the last write. */
    bool is_modified() const { return _modified; }

    /** Returns whether parallax correction is enabled for this probe. */
    bool is_parallax_correction() const;

    /** Sets whether parallax correction is enabled for this probe. */
    void set_parallax_correction(bool parallax_correction);

    /** Returns the border smoothness factor. */
    float get_border_smoothness() const;

    /** Sets the border smoothness factor. */
    void set_border_smoothness(float border_smoothness);

    /** Sets the probe position. */
    void set_pos(const LVecBase3& pos);

    /** Sets the probe rotation. */
    void set_hpr(const LVecBase3& hpr);

    /** Sets the probe scale. */
    void set_scale(const LVecBase3& scale);

    /** Sets the probes matrix, overrides all other transforms. */
    void set_mat(const LMatrix4& matrix);

    /** Updates the spheres bounds. */
    void update_bounds();

    /** Returns the matrix of the probe. */
    const LMatrix4& get_matrix() const;

    /** Writes the probe to a given byte buffer. */
    void write_to_buffer(PTA_uchar& buffer_ptr);

private:
    int _index = -1;
    int _last_update = -1;
    CPT(TransformState) _transform;
    PT(BoundingSphere) _bounds;
    bool _modified = true;
    bool _parallax_correction = true;
    float _border_smoothness = 0.1f;
};

inline void EnvironmentProbe::set_pos(const LVecBase3& pos)
{
    _transform = _transform->set_pos(pos);
    update_bounds();
}

inline void EnvironmentProbe::set_hpr(const LVecBase3& hpr)
{
    _transform = _transform->set_hpr(hpr);
    update_bounds();
}

inline void EnvironmentProbe::set_scale(const LVecBase3& scale)
{
    _transform = _transform->set_scale(scale);
    update_bounds();
}

inline void EnvironmentProbe::set_mat(const LMatrix4& matrix)
{
    _transform = TransformState::make_mat(matrix);
    update_bounds();
}

inline void EnvironmentProbe::update_bounds()
{
    const auto& mat = _transform->get_mat();
    const LVecBase3& mid_point = mat.xform_point(LVecBase3(0, 0, 0));
    const LVecBase3& max_point = mat.xform_point(LVecBase3(1, 1, 1));
    auto radius = (mid_point - max_point).length();
    _bounds = new BoundingSphere(mid_point, radius);
    _modified = true;
}

inline const LMatrix4& EnvironmentProbe::get_matrix() const
{
    return _transform->get_mat();
}

inline BoundingSphere* EnvironmentProbe::get_bounds() const
{
    return _bounds;
}

inline bool EnvironmentProbe::is_parallax_correction() const
{
    return _parallax_correction;
}

inline void EnvironmentProbe::set_parallax_correction(bool parallax_correction)
{
    _parallax_correction = parallax_correction;
}

inline float EnvironmentProbe::get_border_smoothness() const
{
    return _border_smoothness;
}

inline void EnvironmentProbe::set_border_smoothness(float border_smoothness)
{
    _border_smoothness = border_smoothness;
}

}
