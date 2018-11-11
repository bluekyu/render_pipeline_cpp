/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

namespace rpplugins {

class Dialog
{
public:
    Dialog(const std::string& id);

    void open();

    /**
     * @return  Empty if dialog is not closed, and true if dialog is accepted.
     */
    const boost::optional<bool>& draw();

protected:
    virtual void draw_contents();
    virtual void draw_buttons();

    virtual void accept();
    virtual void reject();

    const std::string id_;

private:
    bool will_open_ = false;
    boost::optional<bool> accepted_;
};

inline Dialog::Dialog(const std::string& id) : id_(id)
{
}

// ************************************************************************************************

class MessageDialog : public Dialog
{
public:
    MessageDialog(const std::string& id);

    void set_message(const std::string& msg);

protected:
    void draw_contents() override;

private:
    std::string message_;
};

inline MessageDialog::MessageDialog(const std::string& id) : Dialog(id)
{
}

inline void MessageDialog::set_message(const std::string& msg)
{
    message_ = msg;
}

}
