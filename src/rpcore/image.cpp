#include <render_pipeline/rpcore/image.hpp>

#include <render_pipeline/rpcore/render_target.hpp>

namespace rpcore {

const std::unordered_map<std::string, Image::ComponentFormatType> Image::FORMAT_MAPPING = {
    { "R11G11B10",  { Texture::T_float, Texture::F_r11_g11_b10 } },
    { "RGBA8",      { Texture::T_unsigned_byte, Texture::F_rgba8 } },
    { "RGBA16",     { Texture::T_float, Texture::F_rgba16 } },
    { "RGBA32",     { Texture::T_float, Texture::F_rgba32 } },
    { "R8",         { Texture::T_unsigned_byte, Texture::F_red } },
    { "R8UI",       { Texture::T_unsigned_byte, Texture::F_red } },
    { "R16",        { Texture::T_float, Texture::F_r16 } },
    { "R16UI",      { Texture::T_unsigned_short, Texture::F_r16i } },
    { "R32",        { Texture::T_float, Texture::F_r32 } },
    { "R32I",       { Texture::T_int, Texture::F_r32i } },
};

std::vector<Image*> Image::REGISTERED_IMAGES;

std::shared_ptr<Image> Image::create_buffer(const std::string& name, int size, const std::string& component_format)
{
    auto img = std::make_shared<Image>("ImgBuffer-" + name);
    img->setup_buffer(size, component_format);
    return img;
}

void Image::setup_buffer(int size, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_buffer_texture(size, comp_type_format.first, comp_type_format.second, GeomEnums::UH_static);
}

std::shared_ptr<Image> Image::create_counter(const std::string& name)
{
    return Image::create_buffer(name, 1, "R32I");
}

void Image::setup_counter(void)
{
    Image::setup_buffer(1, "R32I");
}

std::shared_ptr<Image> Image::create_2d(const std::string& name, int w, int h, const std::string& component_format)
{
    auto img = std::make_shared<Image>("Img2D-" + name);
    img->setup_2d(w, h, component_format);
    return img;
}

void Image::setup_2d(int w, int h, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_2d_texture(w, h, comp_type_format.first, comp_type_format.second);
}

std::shared_ptr<Image> Image::create_2d_array(const std::string& name, int w, int h, int slices, const std::string& component_format)
{
    auto img = std::make_shared<Image>(std::string("Img2DArr-") + name);
    img->setup_2d_array(w, h, slices, component_format);
    return img;
}

void Image::setup_2d_array(int w, int h, int slices, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_2d_texture_array(w, h, slices, comp_type_format.first, comp_type_format.second);
}

std::shared_ptr<Image> Image::create_3d(const std::string& name, int w, int h, int slices, const std::string& component_format)
{
    auto img = std::make_shared<Image>(std::string("Img3D-") + name);
    img->setup_3d(w, h, slices, component_format);
    return img;
}

void Image::setup_3d(int w, int h, int slices, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_3d_texture(w, h, slices, comp_type_format.first, comp_type_format.second);
}

std::shared_ptr<Image> Image::create_cube(const std::string& name, int size, const std::string& component_format)
{
    auto img = std::make_shared<Image>(std::string("ImgCube-") + name);
    img->setup_cube(size, component_format);
    return img;
}

void Image::setup_cube(int size, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_cube_map(size, comp_type_format.first, comp_type_format.second);
}

std::shared_ptr<Image> Image::create_cube_array(const std::string& name, int size, int num_cubemaps, const std::string& component_format)
{
    auto img = std::make_shared<Image>(std::string("ImgCubeArr-") + name);
    img->setup_cube_array(size, num_cubemaps, component_format);
    return img;
}

void Image::setup_cube_array(int size, int num_cubemaps, const std::string& component_format)
{
    const auto& comp_type_format = convert_texture_format(component_format);
    texture_->setup_cube_map_array(size, num_cubemaps, comp_type_format.first, comp_type_format.second);
}

const Image::ComponentFormatType& Image::convert_texture_format(const std::string& comp_type)
{
    try
    {
        return FORMAT_MAPPING.at(comp_type);
    }
    catch (const std::out_of_range& err)
    {
        RPObject::global_error("Image", std::string("Unsupported texture component format: ") + comp_type);
        throw err;
    }
}

std::string Image::convert_texture_format(ComponentFormatType comp_format_type)
{
    std::string comp_type = "";

    switch (comp_format_type.first)
    {
    case Texture::T_unsigned_byte:
        {
            switch (comp_format_type.second)
            {
            case Texture::F_rgba8:
                comp_type = "RGBA8";
                break;
            case Texture::F_red:
                comp_type = "R8";
                //comp_type = "R8UI";
                break;
            }
        }
        break;

    case Texture::T_unsigned_short:
        {
            switch (comp_format_type.second)
            {
            case Texture::F_r16i:
                comp_type = "R16UI";
                break;
            }
        }
        break;

    case Texture::T_float:
        {
            switch (comp_format_type.second)
            {
            case Texture::F_r11_g11_b10:
                comp_type = "R11G11B10";
                break;
            case Texture::F_rgba16:
                comp_type = "RGBA16";
                break;
            case Texture::F_rgba32:
                comp_type = "RGBA32";
                break;
            case Texture::F_r16:
                comp_type = "R16";
                break;
            case Texture::F_r32:
                comp_type = "R32";
                break;
            }
        }
        break;

    case Texture::T_int:
        {
            switch (comp_format_type.second)
            {
            case Texture::F_r32i:
                comp_type = "R32I";
                break;
            }
        }
        break;
    };

    if (comp_type.empty())
    {
        RPObject::global_error("Image", std::string("Unsupported texture component & format."));
    }

    return comp_type;
}

Image::Image(const std::string& name): RPObject(name), texture_(new Texture(name))
{
    Image::REGISTERED_IMAGES.push_back(this);
    texture_->set_clear_color(0);
    texture_->clear_image();
    sort_ = RenderTarget::CURRENT_SORT;
}

Image::~Image(void)
{
    Image::REGISTERED_IMAGES.erase(std::find(Image::REGISTERED_IMAGES.begin(), Image::REGISTERED_IMAGES.end(), this));
}

std::string Image::get_texture_format(void) const
{
    return convert_texture_format(ComponentFormatType{ texture_->get_component_type(), texture_->get_format() });
}

}
