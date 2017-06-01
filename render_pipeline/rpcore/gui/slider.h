#pragma once

#include <functional>

#include <nodePath.h>

#include <render_pipeline/rpcore/rpobject.h>

namespace rppanda {
class DirectSlider;
}

namespace rpcore {

/** This is a simple wrapper around DirectSlider, providing a simpler interface. */
class Slider: public RPObject
{
public:
	struct Parameters
	{
		NodePath parent = NodePath();
		float x = 0;
		float y = 0;
		float size = 100;
		float min_value = 0;
		float max_value = 100;
		float value = 50;
		float page_size = 1;
		std::function<void(void*)> callback;
		void* extra_args = nullptr;
	};

public:
	Slider(const Parameters& params=Parameters());
	~Slider(void);

	/** Returns the currently assigned value. */
	float get_value(void) const;

	/** Sets the value of the slider. */
	void set_value(float value);

	/** Returns a handle to the internally used node. */
	rppanda::DirectSlider* get_node(void) const;

private:
	rppanda::DirectSlider* node_;
};

inline rppanda::DirectSlider* Slider::get_node(void) const
{
	return node_;
}

}
