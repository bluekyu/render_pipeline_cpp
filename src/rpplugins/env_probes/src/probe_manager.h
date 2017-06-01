#pragma once

#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpplugins {

class EnvironmentProbe;

/** Manages all environment probes. */
class ProbeManager: public rpcore::RPObject
{
public:
	ProbeManager(void);

	void set_max_probes(int max_probes) { _max_probes = max_probes; }
	void set_resolution(int resolution) { _resolution = resolution; }
	void set_diffuse_resolution(int diffuse_resolution) { _diffuse_resolution = diffuse_resolution; }

	int get_max_probes(void) const;
	int get_resolution(void) const;
	int get_diffuse_resolution(void) const;

	std::shared_ptr<rpcore::Image> get_cubemap_storage(void) const;
	std::shared_ptr<rpcore::Image> get_diffuse_storage(void) const;
	std::shared_ptr<rpcore::Image> get_dataset_storage(void) const;

	/** Creates the cubemap storage. */
	void init(void);

	/** Adds a new probe. */
	bool add_probe(const std::shared_ptr<EnvironmentProbe>& probe);

	/** Updates the manager, updating all probes. */
	void update(void);

	size_t get_num_probes(void) const;

	/** Finds the next probe which requires an update, or returns None. */
	std::shared_ptr<EnvironmentProbe> find_probe_to_update(void) const;

private:
	std::vector<std::shared_ptr<EnvironmentProbe>> _probes;
	int _max_probes = 3;
	int _resolution = 128;
	int _diffuse_resolution = 4;

	std::shared_ptr<rpcore::Image> _cubemap_storage;
	std::shared_ptr<rpcore::Image> _diffuse_storage;
	std::shared_ptr<rpcore::Image> _dataset_storage;
};

inline int ProbeManager::get_max_probes(void) const
{
	return _max_probes;
}

inline int ProbeManager::get_resolution(void) const
{
	return _resolution;
}

inline int ProbeManager::get_diffuse_resolution(void) const
{
	return _diffuse_resolution;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_cubemap_storage(void) const
{
	return _cubemap_storage;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_diffuse_storage(void) const
{
	return _diffuse_storage;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_dataset_storage(void) const
{
	return _dataset_storage;
}

inline size_t ProbeManager::get_num_probes(void) const
{
	return _probes.size();
}

}	// namespace rpplugins
