/**
 * Render Pipeline C++
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <dtoolbase.h>

#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rpcore/image.hpp>

namespace rpplugins {

class EnvironmentProbe;

/** Manages all environment probes. */
class ProbeManager: public rpcore::RPObject
{
public:
    ProbeManager();

    void set_max_probes(int max_probes) { _max_probes = max_probes; }
    void set_resolution(int resolution) { _resolution = resolution; }
    void set_diffuse_resolution(int diffuse_resolution) { _diffuse_resolution = diffuse_resolution; }

    int get_max_probes() const;
    int get_resolution() const;
    int get_diffuse_resolution() const;

    std::shared_ptr<rpcore::Image> get_cubemap_storage() const;
    std::shared_ptr<rpcore::Image> get_diffuse_storage() const;
    std::shared_ptr<rpcore::Image> get_dataset_storage() const;

    /** Creates the cubemap storage. */
    void init();

    /** Adds a new probe. */
    bool add_probe(const std::shared_ptr<EnvironmentProbe>& probe);

    /** Updates the manager, updating all probes. */
    void update();

    size_t get_num_probes() const;

    /** Finds the next probe which requires an update, or returns None. */
    std::shared_ptr<EnvironmentProbe> find_probe_to_update() const;

private:
    std::vector<std::shared_ptr<EnvironmentProbe>> _probes;
    int _max_probes = 3;
    int _resolution = 128;
    int _diffuse_resolution = 4;

    std::shared_ptr<rpcore::Image> _cubemap_storage;
    std::shared_ptr<rpcore::Image> _diffuse_storage;
    std::shared_ptr<rpcore::Image> _dataset_storage;
};

inline int ProbeManager::get_max_probes() const
{
    return _max_probes;
}

inline int ProbeManager::get_resolution() const
{
    return _resolution;
}

inline int ProbeManager::get_diffuse_resolution() const
{
    return _diffuse_resolution;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_cubemap_storage() const
{
    return _cubemap_storage;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_diffuse_storage() const
{
    return _diffuse_storage;
}

inline std::shared_ptr<rpcore::Image> ProbeManager::get_dataset_storage() const
{
    return _dataset_storage;
}

inline size_t ProbeManager::get_num_probes() const
{
    return _probes.size();
}

}    // namespace rpplugins
