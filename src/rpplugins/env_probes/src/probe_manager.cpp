#include "probe_manager.h"

#include <lens.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

#include "environment_probe.h"

namespace rpplugins {

ProbeManager::ProbeManager(void): RPObject("ProbeManager")
{
}

void ProbeManager::init(void)
{
	// Storage for the specular components (with mipmaps)
	_cubemap_storage = rpcore::Image::create_cube_array("EnvmapStorage", _resolution, _max_probes, "RGBA16");
	_cubemap_storage->set_minfilter(SamplerState::FT_linear_mipmap_linear);
	_cubemap_storage->set_magfilter(SamplerState::FT_linear);
	_cubemap_storage->set_clear_color(LColorf(1.0f, 0.0f, 0.1f, 1.0f));
	_cubemap_storage->clear_image();

	// Storage for the diffuse component
	_diffuse_storage = rpcore::Image::create_cube_array("EnvmapDiffStorage", _diffuse_resolution, _max_probes, "RGBA16");
	_diffuse_storage->set_clear_color(LColorf(1.0f, 0.0f, 0.2f, 1.0f));
	_diffuse_storage->clear_image();

	// Data-storage to store all cubemap properties
	_dataset_storage = rpcore::Image::create_buffer("EnvmapData", _max_probes * 5, "RGBA32");
	_dataset_storage->set_clear_color(LColorf(0));
	_dataset_storage->clear_image();
}

bool ProbeManager::add_probe(const std::shared_ptr<EnvironmentProbe>& probe)
{
	if (_probes.size() >= _max_probes)
	{
		error("Cannot attach probe, out of slots!");
		return false;
	}

	probe->set_last_update(-1);
	probe->set_index(_probes.size());
	_probes.push_back(probe);

	return true;
}

void ProbeManager::update(void)
{
	PTA_uchar& buffer_ptr = _dataset_storage->get_texture()->modify_ram_image();
	for (auto& probe: _probes)
	{
		if (probe->is_modified())
			probe->write_to_buffer(buffer_ptr);
	}
}

std::shared_ptr<EnvironmentProbe> ProbeManager::find_probe_to_update(void) const
{
	if (_probes.empty())
		return nullptr;

	PT(GeometricBoundingVolume) view_frustum = DCAST(GeometricBoundingVolume, rpcore::Globals::base->get_cam_lens()->make_bounds());
	view_frustum->xform(rpcore::Globals::base->get_cam().get_transform(rpcore::Globals::base->get_render())->get_mat());

	auto probes = _probes;
	std::sort(probes.begin(), probes.end(), [](const decltype(probes)::value_type& lhs, const decltype(probes)::value_type& rhs) {
		return lhs->get_last_update() < rhs->get_last_update();
	});

	for (const auto& candidate: probes)
	{
		if (view_frustum->contains(candidate->get_bounds()) == BoundingVolume::IF_no_intersection)
			continue;
		return candidate;
	}

	return nullptr;
}

}	// namespace rpplugins
