#include "environment_probe.hpp"

namespace rpplugins {

EnvironmentProbe::EnvironmentProbe(void): RPObject("EnvironmentProbe")
{
    _transform = TransformState::make_identity();
    _bounds = new BoundingSphere(LPoint3f(0), 1.0f);
}

void EnvironmentProbe::update_bounds(void)
{
    const auto& mat = _transform->get_mat();
    const LVecBase3f& mid_point = mat.xform_point(LVecBase3f(0, 0, 0));
    const LVecBase3f& max_point = mat.xform_point(LVecBase3f(1, 1, 1));
    float radius = (mid_point - max_point).length();
    _bounds = new BoundingSphere(mid_point, radius);
    _modified = true;
}

void EnvironmentProbe::write_to_buffer(PTA_uchar& buffer_ptr)
{
    // 4 = sizeof float, 20 = floats per cubemap
    const int bytes_per_probe = 4 * 20;

    std::string string_data;
    string_data.resize(bytes_per_probe);

    float* data = (float*)string_data.data();

    auto mat = _transform->get_mat();
    mat.invert_in_place();

    for (int i=0; i < 4; ++i)
    {
        for (int j=0; j < 3; ++j)
        {
            data[i*3+j] = mat.get_cell(i, j);
        }
    }

    data[12+0] = _index;
    data[12+1] = _parallax_correction ? 1.0f : 0.0f;
    data[12+2] = _border_smoothness;
    data[12+3] = 0.0f;

    data[12+4] = _bounds->get_center().get_x();
    data[12+5] = _bounds->get_center().get_y();
    data[12+6] = _bounds->get_center().get_z();
    data[12+7] = _bounds->get_radius();

    buffer_ptr.set_subdata(_index * bytes_per_probe, bytes_per_probe, string_data);

    _modified = false;
}

}    // namespace rpplugins
