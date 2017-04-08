#pragma once

#include <render_pipeline/rpcore/rpobject.h>

#include <transformState.h>
#include <boundingSphere.h>

namespace rpplugins {

/** Simple class, representing an environment probe. */
class EnvironmentProbe: public rpcore::RPObject
{
public:
	EnvironmentProbe(void);

	int get_index(void) const { return _index; }
	void set_index(int index) { _index = index; }

	int get_last_update(void) const { return _last_update; }
	void set_last_update(int update) { _last_update = update; }

	BoundingSphere* get_bounds(void) const;

	bool is_modified(void) const { return _modified; }

	void set_mat(const LMatrix4f& matrix);

	void set_parallax_correction(bool parallax_correction);
	void set_border_smoothness(float border_smoothness);

	/** Updates the spheres bounds. */
	void update_bounds(void);

	/** Returns the matrix of the probe. */
	const LMatrix4f& get_matrix(void) const;

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

inline const LMatrix4f& EnvironmentProbe::get_matrix(void) const
{
	return _transform->get_mat();
}

inline BoundingSphere* EnvironmentProbe::get_bounds(void) const
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
