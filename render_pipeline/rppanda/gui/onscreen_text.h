/**
 * @file	onscreen_text.h
 *
 * @date	2012-05-28
 * @author	dri
 *
 * OnscreenText module: contains a C++ implementation of the
 *                       OnscreenText Python class
 *
 */

#pragma once

#include <nodePath.h>
#include <textNode.h>

#include <boost/optional.hpp>

#include <render_pipeline/rpcore/config.h>

class TextFont;

namespace rppanda {

class RENDER_PIPELINE_DECL OnscreenText: public NodePath
{
public:
	// These are the styles of text we might commonly see.  They set the
	// overall appearance of the text according to one of a number of
	// pre-canned styles.  You can further customize the appearance of the
	// text by specifying individual parameters as well.
	enum class Style: int
	{
		plain = 1,
		screen_title,
		screen_prompt,
		name_confirm,
		black_on_white
	};

	struct Parameters
	{
		std::string text = "";
		Style style = Style::plain;
		LVecBase2 pos = LVecBase2(0);
		float roll = 0.0f;
		boost::optional<LVecBase2> scale;
		boost::optional<LColor> fg;
		boost::optional<LColor> bg;
		boost::optional<LColor> shadow;
		LVecBase2 shadow_offset = LVecBase2(0.04f, 0.04f);
		boost::optional<LColor> frame;
		boost::optional<TextNode::Alignment> align;
		boost::optional<float> wordwrap;
		boost::optional<int> draw_order;
		bool decal = false;
		TextFont* font = nullptr;
		NodePath parent = NodePath();
		int sort = 0;
		bool may_change = true;
	};

public:
	/**
	 * OnscreenText constructor
	 * Creates a NodePath that allocates a new TextNode which is initialized with `style'.
	 * You should use NodePath::reparent_to() to insert the onscreen text into the aspect 2d
	 * of the scene graph.
	 *
	 * @param[in] style    one of the pre-canned style parameters defined at the
	 *                     head of this file.  This sets up the default values for
	 *                     many parameters.
	 */
	OnscreenText(const Parameters& params=Parameters());

	void cleanup(void);

	void destroy(void) { cleanup(); }

	/**
	 * Reimplementation of TextNode::set_card_decal.
	 * @param[in] decal   if this is true, the text is decalled onto its
	 *                    background card.  Useful when the text will be parented
	 *                    into the 3-D scene graph.
	 */
	void set_decal(bool decal);
	bool get_decal(void) const;

	void set_font(TextFont* font);
	TextFont* get_font(void) const;

	void clear_text();
	void set_text(const std::string& text);
	void append_text(const std::string& text);
	std::string get_text(void) const;

	/** Position the onscreen text on the x-axis. */
	void set_x(float x);
	/** Position the onscreen text on the y-axis. */
	void set_y(float y);
	/** Position the onscreen text in 2d screen space. */
	void set_pos(float x, float y);
	void set_pos(const LVecBase2f& pos);
	const LVecBase2f& get_pos(void) const;

	void set_roll(float roll);
	float get_roll() const;

	void set_scale(float scale);
	void set_scale(float scaleX, float scaleY);

	/**
	 * Scale the text in 2d space.  You may specify either a single
	 * uniform scale, or two scales, or a tuple of two scales.
	 */
	void set_scale(const LVecBase2f& scale);

	void update_transform_mat(void);
	const LVecBase2f& get_scale() const;

	void set_wordwrap(float wordwarp);
	float get_wordwrap(void) const;

	void set_fg(const LColorf& fg);
	void set_bg(const LColorf& bg);
	void set_shadow(const LColor& shadow);
	void set_shadow_offset(const LVecBase2f& offset);
	void set_frame(const LColorf& frame);

	void set_align(TextNode::Alignment align);

private:
	LVecBase2f _scale;
	LVecBase2f _pos;
	float _roll;
	float _wordwrap;
	bool _may_change = true;
	bool _is_clean = false;

	PandaNode* _text_node;		///< This is just for access.
};

// ************************************************************************************************
inline void OnscreenText::set_x(float x)
{
	set_pos(LVecBase2f(x, _pos[1]));
}

inline void OnscreenText::set_y(float y)
{
	set_pos(_pos[0], y);
}

inline void OnscreenText::set_pos(float x, float y)
{
	set_pos(LVecBase2f(x, y));
}

inline const LVecBase2f& OnscreenText::get_pos() const
{
	return _pos;
}

inline float OnscreenText::get_roll() const
{
	return _roll;
}

inline void OnscreenText::set_scale(float scale)
{
	set_scale(LVecBase2f(scale, scale));
}

inline void OnscreenText::set_scale(float scaleX, float scaleY)
{
	set_scale(LVecBase2f(scaleX, scaleY));
}

inline float OnscreenText::get_wordwrap(void) const
{
	return _wordwrap;
}

inline void OnscreenText::update_transform_mat(void)
{
	const LMatrix4f& mat =
		LMatrix4f::scale_mat(_scale.get_x(), 1, _scale.get_y()) *
		LMatrix4f::rotate_mat(_roll, LVector3f::back()) *
		LMatrix4f::translate_mat(_pos.get_x(), 0, _pos.get_y());
	DCAST(TextNode, _text_node)->set_transform(mat);
}

}
