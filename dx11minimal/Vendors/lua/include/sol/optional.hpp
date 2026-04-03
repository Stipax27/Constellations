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

#ifndef SOL_OPTIONAL_HPP
#define SOL_OPTIONAL_HPP

#include <sol/forward.hpp>
#include <sol/in_place.hpp>
#include <sol/traits.hpp>

#include <sol/optional_implementation.hpp>
#include <EASTL/optional.h>

namespace sol {

	namespace meta {
		template <typename T>
		using is_optional = any<is_specialization_of<T, optional>, is_specialization_of<T, eastl::optional>>;

		template <typename T>
		constexpr inline bool is_optional_v = is_optional<T>::value;
	} // namespace meta

	namespace detail {
		template <typename T>
		struct associated_nullopt {
			inline static constexpr eastl::nullopt_t value = eastl::nullopt;
		};

		template <typename T>
		inline constexpr auto associated_nullopt_v = associated_nullopt<T>::value;

	} // namespace detail
} // namespace sol

#endif // SOL_OPTIONAL_HPP
