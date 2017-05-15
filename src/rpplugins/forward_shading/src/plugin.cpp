#include "plugin.hpp"

#include <boost/dll/alias.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

void Plugin::on_stage_setup(void)
{
	_stage = std::make_shared<ForwardStage>(pipeline_);
	add_stage(_stage);

	if (is_plugin_enabled("scattering"))
	{
		_stage->get_required_pipes().push_back("ScatteringIBLSpecular");
		_stage->get_required_pipes().push_back("ScatteringIBLDiffuse");
	}

	if (is_plugin_enabled("pssm"))
	{
		_stage->get_required_pipes().push_back("PSSMSceneSunShadowMapPCF");
		_stage->get_required_inputs().push_back("PSSMSceneSunShadowMVP");
	}

	if (is_plugin_enabled("env_probes"))
	{
		_stage->get_required_pipes().push_back("PerCellProbes");
		_stage->get_required_inputs().push_back("EnvProbes");
	}
}

}
