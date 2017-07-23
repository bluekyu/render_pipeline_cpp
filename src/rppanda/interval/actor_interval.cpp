#include "render_pipeline/rppanda/interval/actor_interval.hpp"

#include "render_pipeline/rppanda/actor/actor.hpp"

namespace rppanda {

int ActorInterval::anim_num_ = 1;

TypeHandle ActorInterval::type_handle_;

ActorInterval::ActorInterval(Actor* actor, const std::string& anim_name, bool loop, bool constrained_loop,
    boost::optional<double> duration): CInterval("unnamed", 0.0f, false)
{
    // Generate unique id
    std::string id = "Actor-" + std::to_string(ActorInterval::anim_num_);
    ActorInterval::anim_num_ += 1;
    // Record class specific variables
    
    actor_ = actor;
}

}
