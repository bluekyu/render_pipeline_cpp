#pragma once

#include "render_pipeline/rpcore/rpobject.h"

#include <render_pipeline/rppanda/gui/onscreen_image.h>

namespace rpcore {

/**
 * Simple wrapper arround OnscreenImage, providing a simpler interface.
 */
class Sprite: public RPObject
{
public:
	/**
	 * Creates a new image, taking (x,y) as topleft coordinates.
	 * When near_filter is set to true, a near filter will be set to the
	 * texture passed.This provides sharper images.
	 * When any_filter is set to false, the passed image won't be modified at
	 * all.This enables you to display existing textures, otherwise the
	 * texture would get a near filter in the 3D View, too.
	 */
	Sprite(Texture* image, int w, int h, NodePath parent=NodePath(), float x=0.0f, float y=0.0f,
		bool transparent=true, bool near_filter=true, bool any_filter=true);
	Sprite(Texture* image, NodePath parent=NodePath(), float x=0.0f, float y=0.0f,
		bool transparent=true, bool near_filter=true, bool any_filter=true);
	Sprite(const std::string& image_path, int w, int h, NodePath parent=NodePath(), float x=0.0f,
		float y=0.0f, bool transparent=true, bool near_filter=true, bool any_filter=true);
	Sprite(const std::string& image_path, NodePath parent=NodePath(), float x=0.0f, float y=0.0f,
		bool transparent=true, bool near_filter=true, bool any_filter=true);

	/** Returns a pos interval, this is a wrapper around NodePath.posInterval. */
	const LVecBase3f& get_initial_pos(void) const;

	/** Returns the width of the image in pixels. */
	int get_width(void) const;

	/** Returns the height of the image in pixels. */
	int get_height(void) const;

	/** Sets the position. */
	void set_pos(float x, float y);

	/** Sets a shader to be used for rendering the image. */
	void set_shader(const Shader* shader, int priority=0);

	/** Sets a shader input on the image. */
	void set_shader_input(const ShaderInput& inp);

	/** Hides the image. */
	void hide(void);

	/** Shows the image if it was previously hidden. */
	void show(void);

	/** Returns whether the image is hidden. */
	bool is_hidden(void) const;

	rppanda::OnscreenImage get_node(void) const;

private:
	void init(Texture* image, int w, int h, NodePath parent, float x, float y,
		bool transparent, bool near_filter, bool any_filter);
	LVecBase3f translate_pos(float x, float y) const;

	int _width;
	int _height;
	LVecBase3f _initial_pos;

	rppanda::OnscreenImage node;
};

// ************************************************************************************************
inline Sprite::Sprite(Texture* image, NodePath parent, float x, float y,
	bool transparent, bool near_filter, bool any_filter):
	Sprite(image, 10, 10, parent, x, y, transparent, near_filter, any_filter)
{
}

inline LVecBase3f Sprite::translate_pos(float x, float y) const
{
	return LVecBase3f(x + _width / 2.0f, 1, -y - _height / 2.0f);
}

inline const LVecBase3f& Sprite::get_initial_pos(void) const
{
	return _initial_pos;
}

inline int Sprite::get_width(void) const
{
	return _width;
}

inline int Sprite::get_height(void) const
{
	return _height;
}

inline void Sprite::set_pos(float x, float y)
{
	node.set_pos(translate_pos(x, y));
}

inline void Sprite::set_shader(const Shader* shader, int priority)
{
	node.set_shader(shader, priority);
}

inline void Sprite::set_shader_input(const ShaderInput& inp)
{
	node.set_shader_input(inp);
}

inline void Sprite::hide(void)
{
	node.hide();
}

inline void Sprite::show(void)
{
	node.show();
}

inline bool Sprite::is_hidden(void) const
{
	return node.is_hidden();
}

inline rppanda::OnscreenImage Sprite::get_node(void) const
{
	return node;
}

}	// namespace rpcore
