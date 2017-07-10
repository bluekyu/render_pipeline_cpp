#include "rpcore/gui/error_message_display.hpp"

#include <lineStream.h>
#include <pnotify.h>

#include <boost/algorithm/string.hpp>

#include "render_pipeline/rpcore/gui/text.hpp"
#include "render_pipeline/rpcore/globals.hpp"
#include "render_pipeline/rppanda/showbase/showbase.hpp"

namespace rpcore {

ErrorMessageDisplay::ErrorMessageDisplay(void): RPObject("ErrorMessageDisplay")
{
	_error_node = Globals::base->get_pixel_2d().attach_new_node("ErrorDisplay");
	_error_node.set_z(-180);
}

ErrorMessageDisplay::~ErrorMessageDisplay(void)
{
	delete _notify_stream;
}

void ErrorMessageDisplay::update(void)
{
	if (!_notify_stream)
		init_notify();

	while (_notify_stream->is_text_available())
	{
		const std::string line(_notify_stream->get_line());
		if (line.find("warning") != std::string::npos)
		{
			RPObject::global_warn("Panda3d", line);
		}
		else if (line.find("error") != std::string::npos)
		{
			RPObject::global_error("Panda3d", line);
			add_error(line);
		}
		else
		{
			RPObject::global_debug("Panda3d", line);
		}
	}
}

void ErrorMessageDisplay::add_error(const std::string& msg)
{
	add_text(msg, LVecBase3f(1.0f, 0.2f, 0.2f));
}

void ErrorMessageDisplay::add_warning(const std::string& msg)
{
	add_text(msg, LVecBase3f(1.0f, 1.0f, 0.2f));
}

void ErrorMessageDisplay::add_text(const std::string& text, const LVecBase3f& color)
{
	Text::Parameters params;
	params.text = boost::trim_copy(text);
	params.parent = _error_node;
	params.x = Globals::native_resolution.get_x() - 30;
	params.y = _num_errors * 23;
	params.size = 12.0f;
	params.align = "right";
	params.color = color;
	Text error_text(params);

	_num_errors += 1;

	if (_num_errors > 30)
	{
		clear_messages();
		add_error("Error count exceeded. Cleared errors ..");
	}
}

void ErrorMessageDisplay::clear_messages(void)
{
	_error_node.node()->remove_all_children();
	_num_errors = 0;
}

void ErrorMessageDisplay::init_notify(void)
{
	_notify_stream = new LineStream;
	Notify::ptr()->set_ostream_ptr(_notify_stream, false);
}

}
