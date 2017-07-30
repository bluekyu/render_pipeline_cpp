/**
 * Copyright (c) 2008, Carnegie Mellon University.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Carnegie Mellon University nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * (This is the Modified BSD License.  See also
 * http://www.opensource.org/licenses/bsd-license.php )
 */

/**
 * This is C++ porting codes of direct/src/gui/OnscreeImage.py
 */

#include <render_pipeline/rppanda/gui/onscreen_image.hpp>

#include <texturePool.h>
#include <cardMaker.h>
#include <pandaFramework.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rppanda {

TypeHandle OnscreenImage::type_handle_;

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
