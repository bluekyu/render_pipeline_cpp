#pragma once

#include <nodePath.h>

#include <render_pipeline/rpcore/config.hpp>

namespace rpcore {

class RENDER_PIPELINE_DECL CircularPointsNode
{
public:
    CircularPointsNode(const std::string& name, const std::vector<LPoint3f>& positions, float radius=1.0f,
        const std::string& effect_path="", GeomEnums::UsageHint buffer_hint=GeomEnums::UH_static);

    ~CircularPointsNode(void);

    NodePath get_nodepath(void) const;

    /** Get the count of instances. */
    int get_point_count(void) const;

    /** Get position. */
    const LPoint3f& get_position(int point_index) const;

    /** Get local instancing tranforms. */
    const std::vector<LPoint3f>& get_positions(void) const;

    LPoint3f* modify_positions(void);

    /** Set local instancing transform. */
    void set_position(const LPoint3f& positions, int point_index);

    /** Set local instancing transforms and change instance count to the size of @p transforms. */
    void set_positions(const std::vector<LPoint3f>& positions);

    /** Upload transform buffer texture to GPU. */
    void upload_positions(void);

    void set_radius(float radius);

    /**
     * Set a count of active points.
     *
     * Panda3D will draw points as given counts.
     * If @a count is -1, all points will be used.
     */
    void set_active_point_count(int count);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}

