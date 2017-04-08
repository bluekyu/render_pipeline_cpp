#include "rpcore/util/ies_profile_loader.h"

#include <shaderInput.h>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/stage_manager.h>

namespace rpcore {

const std::vector<std::string> IESProfileLoader::PROFILES = {
	"IESNA:LM-63-1986",
	"IESNA:LM-63-1991",
	"IESNA91",
	"IESNA:LM-63-1995",
	"IESNA:LM-63-2002",
	"ERCO Leuchten GmbH  BY: ERCO/LUM650/8701",
	"ERCO Leuchten GmbH"
};

size_t IESProfileLoader::load(const std::string& filename)
{
	// TODO: implement
	return 0;
}

void IESProfileLoader::create_storage(void)
{
    _storage_tex = Image::create_3d("IESDatasets", 512, 512, _max_entries, "R16");
    _storage_tex->set_minfilter(SamplerState::FT_linear);
    _storage_tex->set_magfilter(SamplerState::FT_linear);
    _storage_tex->set_wrap_u(SamplerState::WM_clamp);
    _storage_tex->set_wrap_v(SamplerState::WM_repeat);
    _storage_tex->set_wrap_w(SamplerState::WM_clamp);

    _pipeline->get_stage_mgr()->add_input(ShaderInput("IESDatasetTex", _storage_tex->get_texture()));
    _pipeline->get_stage_mgr()->get_defines()["MAX_IES_PROFILES"] = std::to_string(_max_entries);
}

}	// namespace rpcore
