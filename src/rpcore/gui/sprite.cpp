#include <render_pipeline/rpcore/gui/sprite.h>

#include <render_pipeline/rpcore/loader.hpp>

namespace rpcore {

Sprite::Sprite(Texture* image, int w, int h, NodePath parent, float x, float y,
	bool transparent, bool near_filter, bool any_filter): RPObject("Sprite")
{
	init(image, w, h, parent, x, y, transparent, near_filter, any_filter);
}

Sprite::Sprite(const std::string& image_path, int w, int h, NodePath parent, float x, float y,
	bool transparent, bool near_filter, bool any_filter):
	Sprite(RPLoader::load_texture(image_path), w, h, parent, x, y, transparent, near_filter, any_filter)
{
}

Sprite::Sprite(const std::string& image_path, NodePath parent, float x, float y,
	bool transparent, bool near_filter, bool any_filter): RPObject("Sprite")
{
	Texture* tex = RPLoader::load_texture(image_path);
	init(tex, tex->get_x_size(), tex->get_y_size(), parent, x, y, transparent, near_filter, any_filter);
}

void Sprite::init(Texture* image, int w, int h, NodePath parent, float x, float y,
	bool transparent, bool near_filter, bool any_filter)
{
	_width = w;
	_height = h;

	_initial_pos = translate_pos(x, y);

	node = rppanda::OnscreenImage(std::make_shared<rppanda::ImageInput>(image), parent);
	node.set_pos(_initial_pos);
	node.set_scale(_width / 2.0f, 1.0f, _height / 2.0f);

	if (transparent)
		node.set_transparency(TransparencyAttrib::M_alpha);

	Texture* tex = node.get_texture();

	// Apply a near filter, but only if the parent has no scale, otherwise
	// it will look weird
	if (near_filter && any_filter && parent.get_sx() == 1.0f)
	{
		tex->set_minfilter(SamplerState::FT_nearest);
		tex->set_magfilter(SamplerState::FT_nearest);
	}

	if (any_filter)
	{
		tex->set_anisotropic_degree(8);
		tex->set_wrap_u(SamplerState::WM_clamp);
		tex->set_wrap_v(SamplerState::WM_clamp);
	}
}

}
