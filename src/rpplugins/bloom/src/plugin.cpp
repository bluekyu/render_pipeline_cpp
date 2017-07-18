#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::Plugin)

namespace rpplugins {

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

void Plugin::on_stage_setup(void)
{
    _bloom_stage = std::make_shared<BloomStage>(pipeline_);
    add_stage(_bloom_stage);

    _bloom_stage->set_num_mips(boost::any_cast<int>(get_setting("num_mipmaps")));
    _bloom_stage->set_remove_fireflies(boost::any_cast<bool>(get_setting("remove_fireflies")));
}

void Plugin::on_pipeline_created(void)
{
    Texture* dirt_tex = rpcore::RPLoader::load_texture(get_resource("lens_dirt.txo"));
    _bloom_stage->set_shader_input(ShaderInput("LensDirtTex", dirt_tex));
}

}
