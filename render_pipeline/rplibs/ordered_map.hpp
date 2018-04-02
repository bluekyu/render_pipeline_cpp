/**
 * Render Pipeline C++
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

namespace rplibs {

template <class key_type, class mapped_type>
struct OrderedMapContainerType
{
    key_type key;
    mapped_type value;
};

template <class key_type, class mapped_type>
using OrderedMap = boost::multi_index_container<
    OrderedMapContainerType<key_type, mapped_type>,
    boost::multi_index::indexed_by<
        boost::multi_index::random_access<>,
        boost::multi_index::hashed_unique<
            boost::multi_index::member<
                OrderedMapContainerType<key_type, mapped_type>,
                key_type,
                &OrderedMapContainerType<key_type, mapped_type>::key
            >
        >
    >
>;

}
