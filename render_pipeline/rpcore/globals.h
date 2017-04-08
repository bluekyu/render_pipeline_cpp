#pragma once

#include <nodePath.h>
#include <textFont.h>

#include <render_pipeline/rpcore/config.h>

namespace rppanda {
class ShowBase;
}

namespace rpcore {

/**
 * This class is a singleton to store globals widely used by the application.
 * This is a wrapper around Panda3D's globals since ShowBase writes to __builtins__
 * which is bad practice. This class also attempts to help IDEs to figure out
 * where the variables come from and where they are defined.
 */
class RPCPP_DECL Globals
{
public:
	static void load(rppanda::ShowBase* showbase);

	static rppanda::ShowBase* base;
	static NodePath render;
	static ClockObject* clock;
	static PT(TextFont) font;
	static LVecBase2i resolution;
	static LVecBase2i native_resolution;
};

}	// namespace rpcore
