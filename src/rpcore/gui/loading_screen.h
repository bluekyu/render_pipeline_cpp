#pragma once

#include <render_pipeline/rpcore/rpobject.hpp>

#include <nodePath.h>

namespace rpcore {

class RenderPipeline;
class Sprite;

class LoadingScreen: public RPObject
{
public:
	/**
	 * Inits the loading screen with a given image source. By default,
	 * this is the pipeline loading screen, but it can be overridden.
	 */
	LoadingScreen(RenderPipeline* pipeline, const std::string& image_source="/$$rp/data/gui/loading_screen_bg.txo");
	~LoadingScreen(void);

	/** Creates the gui components. */
	void create(void);

	/** Removes the loading screen/ */
	void remove(void);

private:
	RenderPipeline* pipeline;
	const std::string image_source;

	NodePath fullscreen_node;
	Sprite* fullscreen_bg = nullptr;
};

}
