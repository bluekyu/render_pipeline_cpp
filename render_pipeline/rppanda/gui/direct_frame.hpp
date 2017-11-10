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
 * This is C++ porting codes of direct/src/gui/DirectFrame.py
 */

#pragma once

#include <render_pipeline/rppanda/gui/direct_gui_base.hpp>
#include <render_pipeline/rppanda/util/image_input.hpp>

namespace rppanda {

class RENDER_PIPELINE_DECL DirectFrame : public DirectGuiWidget
{
public:
    /** @see DirectGuiWidget::Options */
    struct RENDER_PIPELINE_DECL Options : public DirectGuiWidget::Options
    {
        Options();

        std::vector<std::string> text;
        std::vector<std::shared_ptr<ImageInput>> image;

        bool text_may_change = true;
    };

public:
    DirectFrame(NodePath parent={}, const std::shared_ptr<Options>& options=std::make_shared<Options>());

    ALLOC_DELETED_CHAIN(DirectFrame);

    const std::vector<std::string>& get_text() const;

    /** Prepare to change text. */
    void prepare_text(const std::string& text);
    void prepare_text(const std::vector<std::string>& text_list);

    /** Apply text changes. */
    void set_text();

    /** Change text directly. */
    void set_text(const std::string& text);
    void set_text(const std::vector<std::string>& text_list);

    const std::vector<std::shared_ptr<ImageInput>>& get_image() const;

    /** Prepare to change image. */
    void prepare_image(const std::shared_ptr<ImageInput>& image);
    void prepare_image(const std::vector<std::shared_ptr<ImageInput>>& images);

    /** Apply image changes. */
    void set_image();

    /** Change image directly. */
    void set_image(const std::shared_ptr<ImageInput>& image);
    void set_image(const std::vector<std::shared_ptr<ImageInput>>& images);

protected:
    DirectFrame(PGItem* gui_item, NodePath parent, const std::shared_ptr<Options>& options, const TypeHandle& type_handle);

    void initialise_options(const std::shared_ptr<Options>& options);

private:
    const std::shared_ptr<Options>& define_options(const std::shared_ptr<Options>& options);

public:
    static TypeHandle get_class_type();
    static void init_type();
    virtual TypeHandle get_type() const;
    virtual TypeHandle force_init_type();

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline void DirectFrame::set_text(const std::string& text)
{
    prepare_text(text);
    set_text();
}

inline void DirectFrame::set_text(const std::vector<std::string>& text_list)
{
    prepare_text(text_list);
    set_text();
}

inline void DirectFrame::set_image(const std::shared_ptr<ImageInput>& image)
{
    prepare_image(image);
    set_image();
}

inline void DirectFrame::set_image(const std::vector<std::shared_ptr<ImageInput>>& images)
{
    prepare_image(images);
    set_image();
}

inline TypeHandle DirectFrame::get_class_type()
{
    return type_handle_;
}

inline void DirectFrame::init_type()
{
    DirectGuiWidget::init_type();
    register_type(type_handle_, "rppanda::DirectFrame", DirectGuiWidget::get_class_type());
}

inline TypeHandle DirectFrame::get_type() const
{
    return get_class_type();
}

inline TypeHandle DirectFrame::force_init_type()
{
    init_type();
    return get_class_type();
}

}
