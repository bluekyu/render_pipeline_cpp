#include "render_pipeline/rpcore/loader.hpp"

#include <chrono>

#include <textFont.h>
#include <pnmImage.h>
#include <pandaFramework.h>
#include <fontPool.h>
#include <texturePool.h>
#include <virtualFileMountRamdisk.h>

#include <boost/algorithm/string/join.hpp>

#include <spdlog/fmt/ostr.h>

#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"
#include "render_pipeline/rppanda/stdpy/file.hpp"

namespace rpcore {

/**
 * Context manager for a synchronous loading operation, keeping track
 * on how much time elapsed during the loading process, and warning about
 * long loading times.
 */
class TimedLoadingOperation
{
public:
    static int WARNING_COUNT;

    TimedLoadingOperation(const std::string& resource): resource_(resource)
    {
        enter();
    }

    ~TimedLoadingOperation(void)
    {
        exit();
    }

private:
    void enter(void)
    {
        start_time_ = std::chrono::system_clock::now();
    }

    void exit(void)
    {
        float duration = (std::chrono::duration<float>(std::chrono::system_clock::now() - start_time_)).count();
        if (duration > 80.0f && WARNING_COUNT < 5)
        {
            RPObject::global_warn("RPLoader",
                fmt::format("Loading '{}' took {} ms", resource_, (std::round(duration * 100.0f) / 100.0f)));
            ++WARNING_COUNT;
            if (WARNING_COUNT == 5)
            {
                RPObject::global_warn("RPLoader",
                    "Skipping further loading warnings (max warning count reached)");
            }
        }
    }

    std::string resource_;
    std::chrono::system_clock::time_point start_time_;
};

int TimedLoadingOperation::WARNING_COUNT = 0;

// ************************************************************************************************
Texture* RPLoader::load_texture(const Filename& filename)
{
    TimedLoadingOperation tlo(filename);

    return TexturePool::load_texture(filename);
}

Texture* RPLoader::load_cube_map(const Filename& filename, bool read_mipmaps)
{
    TimedLoadingOperation tlo(filename);

    return TexturePool::load_cube_map(filename, read_mipmaps);
}

Texture* RPLoader::load_3d_texture(const Filename& filename)
{
    TimedLoadingOperation tlo(filename);

    return TexturePool::load_3d_texture(filename);
}

PT(Shader) RPLoader::load_shader(const std::vector<Filename>& path_args)
{
    TimedLoadingOperation tlo(rppanda::join_to_string(path_args));

    const size_t len = path_args.size();

    if (len == 1)
        return Shader::load_compute(Shader::SL_GLSL, path_args[0]);

    return Shader::load(
        Shader::SL_GLSL,				// ShaderLanguage
        path_args[0],					// vertex
        path_args[1],					// fragment
        len > 2 ? path_args[2] : "",	// geometry
        len > 3 ? path_args[3] : "",
        len > 4 ? path_args[4] : "");
}

TextFont* RPLoader::load_font(const Filename& filename)
{
    TimedLoadingOperation tlo(filename);

    return FontPool::load_font(filename);
}

NodePath RPLoader::load_model(const Filename& filename)
{
    TimedLoadingOperation tlo(filename);

    return Globals::base->get_window_framework()->load_model(Globals::base->get_panda_framework()->get_models(), filename);
}

Texture* RPLoader::load_sliced_3d_texture(const Filename& filename, int tile_size_x)
{
    return RPLoader::load_sliced_3d_texture(filename, tile_size_x, tile_size_x, tile_size_x);
}

Texture* RPLoader::load_sliced_3d_texture(const Filename& filename, int tile_size_x, int tile_size_y)
{
    return RPLoader::load_sliced_3d_texture(filename, tile_size_x, tile_size_y, tile_size_x);
}

Texture* RPLoader::load_sliced_3d_texture(const Filename& filename, int tile_size_x, int tile_size_y, int num_tiles)
{
    TimedLoadingOperation tlo(filename);

    double epoch_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
    const std::string tempfile_name = std::string("/$$slice_loader_temp-") + std::to_string(epoch_time) + "/";

    // Load sliced image from disk
    PT(Texture) tex_handle = RPLoader::load_texture(filename);
    PNMImage source;
    tex_handle->store(source);
    int width = source.get_x_size();

    // Find slice properties
    int num_cols = width / tile_size_x;
    PNMImage temp_img(tile_size_x, tile_size_y, source.get_num_channels(), source.get_maxval());

    // Construct a ramdisk to write the files to
    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
    PT(VirtualFileMountRamdisk) ramdisk = new VirtualFileMountRamdisk;
    vfs->mount(ramdisk, tempfile_name, 0);

    // Extract all slices and write them to the virtual disk
    for (int z_slice=0; z_slice < num_tiles; ++z_slice)
    {
        int slice_x = (z_slice % num_cols) * tile_size_x;
        int slice_y = (z_slice / num_cols) * tile_size_y;
        temp_img.copy_sub_image(source, 0, 0, slice_x, slice_y, tile_size_x, tile_size_y);
        temp_img.write(tempfile_name + std::to_string(z_slice) + ".png");
    }

    // Load the de-sliced texture from the ramdisk
    Texture* texture_handle = RPLoader::load_3d_texture(tempfile_name + "/#.png");
    vfs->unmount(ramdisk);

    return texture_handle;
}

}
