#include <render_pipeline/rppanda/gui/onscreen_image.h>

#include <texturePool.h>
#include <cardMaker.h>
#include <pandaFramework.h>

#include <render_pipeline/rppanda/showbase/showbase.h>

namespace rppanda {

void OnscreenImage::set_image(const std::shared_ptr<ImageInput>& image, NodePath parent, const TransformState* transform, int sort)
{
	// Get the original parent, transform, and sort, if any, so we can
	// preserve them across this call.
	if (!is_empty())
	{
		parent = get_parent();

		if (!transform)
			transform = get_transform();
		sort = get_sort();
	}

	remove_node();

	// Assign geometry
	if (image)
	{
		switch (image->get_image_input_type())
		{
			case ImageInput::ImageInputType::NODEPATH:
			{
				// Assign geometry
				NodePath::operator=(std::move(image->get_nodepath().copy_to(parent, sort)));
				break;
			}

			case ImageInput::ImageInputType::STRING:
			case ImageInput::ImageInputType::TEXTURE:
			{
				Texture* tex;
				if (image->is_texture())
					tex = image->get_texture();
				else
					tex = TexturePool::load_texture(image->get_string());

				CardMaker cm = CardMaker("OnscreenImage");
				cm.set_frame(-1, 1, -1, 1);
				NodePath::operator=(std::move(parent.attach_new_node(cm.generate(), sort)));
				set_texture(tex);

				break;
			}

			case ImageInput::ImageInputType::FILE_NODE:
			{
				NodePath model = ShowBase::get_global_ptr()->get_window_framework()->load_model(
					ShowBase::get_global_ptr()->get_panda_framework()->get_models(), image->get_file_node().first);

				if (!model.is_empty())
				{
					NodePath node = model.find(image->get_file_node().second);
					if (!node.is_empty())
						NodePath::operator=(node.copy_to(parent, sort));
					else
						std::cout << "OnscreenImage: node " << image->get_file_node().second << " not found" << std::endl;
				}
				else
				{
					std::cout << "OnscreenImage: model " << image->get_file_node().first << " not found" << std::endl;
				}

				break;
			}

			default:
				break;
		}
	}

	if (transform && !is_empty())
		set_transform(transform);
}

}
