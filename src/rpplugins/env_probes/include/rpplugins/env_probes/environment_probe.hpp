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

    bool is_modified() const { return _modified; }

    void set_mat(const LMatrix4f& matrix);

    void set_parallax_correction(bool parallax_correction);
    void set_border_smoothness(float border_smoothness);

    /** Updates the spheres bounds. */
    void update_bounds();

    /** Returns the matrix of the probe. */
    const LMatrix4f& get_matrix() const;

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

inline void EnvironmentProbe::set_mat(const LMatrix4f& matrix)
{
    _transform = TransformState::make_mat(matrix);
    update_bounds();
}

inline const LMatrix4f& EnvironmentProbe::get_matrix() const
{
    return _transform->get_mat();
}

inline BoundingSphere* EnvironmentProbe::get_bounds() const
{
    return _bounds;
}

inline void EnvironmentProbe::set_parallax_correction(bool parallax_correction)
{
    _parallax_correction = parallax_correction;
}

inline void EnvironmentProbe::set_border_smoothness(float border_smoothness)
{
    _border_smoothness = border_smoothness;
}

} // namespace rpplugins
