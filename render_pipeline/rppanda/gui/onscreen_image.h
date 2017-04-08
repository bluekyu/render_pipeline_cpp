#pragma once

#include <nodePath.h>
#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rppanda/util/image_input.h>

namespace rppanda {

class RPCPP_DECL OnscreenImage: public DirectObject, public NodePath
{
public:
	OnscreenImage(NodePath parent=NodePath(), int sort=0);
	OnscreenImage(const std::shared_ptr<ImageInput>& image, NodePath parent=NodePath(), int sort=0);

	void set_image(const std::shared_ptr<ImageInput>& image, NodePath parent=NodePath(), const TransformState* transform=nullptr, int sort=0);

	void destroy(void);
};

// ************************************************************************************************
inline OnscreenImage::OnscreenImage(NodePath parent, int sort)
{
	if (parent.is_empty())
		parent = ShowBase::get_global_ptr()->get_aspect_2d();
	set_image(nullptr, parent, nullptr, sort);
}

inline OnscreenImage::OnscreenImage(const std::shared_ptr<ImageInput>& image, NodePath parent, int sort)
{
	if (parent.is_empty())
		parent = ShowBase::get_global_ptr()->get_aspect_2d();
	set_image(image, parent, nullptr, sort);
}

inline void OnscreenImage::destroy(void)
{
	remove_node();
}

}	// namespace rppanda
