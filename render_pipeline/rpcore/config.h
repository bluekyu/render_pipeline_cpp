#pragma once

#include <boost/config.hpp>

#if defined(RPCPP_BUILD)
#   define RPCPP_DECL BOOST_SYMBOL_EXPORT
#else
#   define RPCPP_DECL BOOST_SYMBOL_IMPORT
#endif
