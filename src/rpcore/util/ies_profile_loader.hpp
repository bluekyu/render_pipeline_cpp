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

#include <vector>
#include <memory>

#include <render_pipeline/rpcore/rpobject.hpp>

class Filename;

namespace rpcore {

class RenderPipeline;
class Image;

/**
 * Loader class to load .IES files and create an IESDataset from it.
 * It generates a LUT for each loaded ies profile which is used by the lighting
 * pipeline later on.
 */
class IESProfileLoader : public RPObject
{
public:
    /**< Supported IES Profiles. */
    static const std::vector<std::string> PROFILES;

    IESProfileLoader(RenderPipeline& pipeline);
    ~IESProfileLoader();

    /**
     * Loads a profile from a given filename and returns the internal
     * used index which can be assigned to a light.
     */
    size_t load(const Filename& filename);

private:
    /** Internal method to create the storage for the profile dataset textures. */
    void create_storage();

    RenderPipeline& pipeline_;
    int max_entries_ = 32;
    std::unique_ptr<Image> storage_tex_;
};

inline IESProfileLoader::IESProfileLoader(RenderPipeline& pipeline): RPObject("IESProfileLoader"), pipeline_(pipeline)
{
    create_storage();
}

}
