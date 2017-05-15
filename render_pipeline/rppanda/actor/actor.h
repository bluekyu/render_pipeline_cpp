#pragma once

#include <nodePath.h>

#include <render_pipeline/rppanda/showbase/direct_object.h>

class Loader;

namespace rppanda {

/**
 * Actor class: Contains methods for creating, manipulating
 * and playing animations on characters.
 */
class RENDER_PIPELINE_DECL Actor: public DirectObject, public NodePath
{
public:
	Actor(void);

private:
	Loader* loader_;
};

}	// namespace rppanda
