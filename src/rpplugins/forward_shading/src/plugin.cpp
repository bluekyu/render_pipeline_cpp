#include "plugin.hpp"

#include <boost/dll/alias.hpp>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
    "Forward Rendering",
    "tobspr <tobias.springer1@gmail.com>",
    "0.1 alpha (!)",

    "This plugin adds support for an additional forward rendering "
    "pass. This is mainly useful for transparency."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpplugins::Plugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info)
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
