// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_STRING_VIEW_HPP
#define SOL_STRING_VIEW_HPP

#include <sol/version.hpp>

#include <cstddef>
#include <EASTL/string.h>
#include <EASTL/string_view.h>
#include <EASTL/functional.h>

namespace sol
{
	template <typename C>
	using basic_string_view = eastl::basic_string_view<C>;

	typedef eastl::string_view string_view;
	typedef eastl::wstring_view wstring_view;
	typedef eastl::u16string_view u16string_view;
	typedef eastl::u32string_view u32string_view;
	typedef eastl::hash<eastl::string_view> string_view_hash;
} // namespace sol

#endif // SOL_STRING_VIEW_HPP
