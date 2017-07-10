#pragma once

#include <boost/config.hpp>

#if defined(RENDER_PIPELINE_BUILD)
#   define RENDER_PIPELINE_DECL BOOST_SYMBOL_EXPORT
#else
#   define RENDER_PIPELINE_DECL BOOST_SYMBOL_IMPORT
#endif
