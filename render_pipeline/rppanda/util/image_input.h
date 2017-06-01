#pragma once

#include <nodePath.h>
#include <texture.h>

#include <render_pipeline/rpcore/config.h>

namespace rppanda {

class RENDER_PIPELINE_DECL ImageInput
{
public:
    enum class ImageInputType: int
    {
        NONE,
        NODEPATH,
        TEXTURE,
        STRING,
        FILE_NODE,
    };

public:
    ImageInput(void): _type(ImageInputType::NONE) {}
    ImageInput(NodePath nodepath): _type(ImageInputType::NODEPATH), _nodepath(nodepath) {}
    ImageInput(Texture* tex): _type(ImageInputType::TEXTURE), _texture(tex) {}
    ImageInput(const std::string& path): _type(ImageInputType::STRING), _filepath(path) {}

    ImageInput(const std::string& file, const std::string& node): _type(ImageInputType::FILE_NODE), _file_node({file, node}) {}
    ImageInput(const std::pair<std::string, std::string>& file_node): _type(ImageInputType::FILE_NODE), _file_node(file_node) {}

    bool is_type(ImageInputType type) const { return this->_type == type; }
    bool is_none(void) const { return _type == ImageInputType::NONE; }
    bool is_nodepath(void) const { return _type == ImageInputType::NODEPATH; }
    bool is_texture(void) const { return _type == ImageInputType::TEXTURE; }
    bool is_string(void) const { return _type == ImageInputType::STRING; }
    bool is_file_node(void) const { return _type == ImageInputType::FILE_NODE; }

    ImageInputType get_image_input_type(void) const { return _type; }
    NodePath get_nodepath(void) const { return _nodepath; }
    Texture* get_texture(void) const { return _texture; }
    const std::string& get_string(void) const { return _filepath; }
    const std::pair<std::string, std::string>& get_file_node(void) const { return _file_node; }

    void set_image(NodePath nodepath) { _type = ImageInputType::NODEPATH; _nodepath = nodepath; }
    void set_image(Texture* tex) { _type = ImageInputType::TEXTURE; _texture = tex; }
    void set_image(const std::string& path) { _type = ImageInputType::STRING; _filepath = path; }

    void set_image(const std::string& file, const std::string& node) { _type = ImageInputType::FILE_NODE; _file_node = std::make_pair(file, node); }
    void set_image(const std::pair<std::string, std::string>& file_node) { _type = ImageInputType::FILE_NODE; _file_node = file_node; }

private:
    ImageInputType _type;

    NodePath _nodepath;
    PT(Texture) _texture = nullptr;
    std::string _filepath;
    std::pair<std::string, std::string> _file_node;
};

}
