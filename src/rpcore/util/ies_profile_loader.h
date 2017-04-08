#pragma once

#include <vector>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/image.h>

namespace rpcore {

class RenderPipeline;

/**
 * Loader class to load .IES files and create an IESDataset from it.
 * It generates a LUT for each loaded ies profile which is used by the lighting
 * pipeline later on.
 */
class IESProfileLoader: public RPObject
{
public:
	/**< Supported IES Profiles. */
	static const std::vector<std::string> PROFILES;

	IESProfileLoader(RenderPipeline* pipeline);

	/**
	 * Loads a profile from a given filename and returns the internal
	 * used index which can be assigned to a light.
	 */
	size_t load(const std::string& filename);

private:
	/** Internal method to create the storage for the profile dataset textures. */
	void create_storage(void);

	RenderPipeline* _pipeline;
	int _max_entries = 32;
	std::shared_ptr<Image> _storage_tex;
};

inline IESProfileLoader::IESProfileLoader(RenderPipeline* pipeline): RPObject("IESProfileLoader"), _pipeline(pipeline)
{
	create_storage();
}

}	// namespace rpcore
