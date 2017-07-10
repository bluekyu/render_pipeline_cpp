/**
 * This is C++ porting codes of direct/src/actor/Actor.py
 */

#pragma once

#include <nodePath.h>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>

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

}
