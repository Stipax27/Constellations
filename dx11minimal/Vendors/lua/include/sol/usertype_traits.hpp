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

#ifndef SOL_USERTYPE_TRAITS_HPP
#define SOL_USERTYPE_TRAITS_HPP

#include <sol/demangle.hpp>

namespace sol {

	template <typename T>
	struct usertype_traits {
		static const eastl::string& name() {
			static const eastl::string& n = detail::short_demangle<T>();
			return n;
		}
		static const eastl::string& qualified_name() {
			static const eastl::string& q_n = detail::demangle<T>();
			return q_n;
		}
		static const eastl::string& metatable() {
			static const eastl::string m = eastl::string("sol.").append(detail::demangle<T>());
			return m;
		}
		static const eastl::string& user_metatable() {
			static const eastl::string u_m = eastl::string("sol.").append(detail::demangle<T>()).append(".user");
			return u_m;
		}
		static const eastl::string& user_gc_metatable() {
			static const eastl::string u_g_m = eastl::string("sol.").append(detail::demangle<T>()).append(".user\xE2\x99\xBB");
			return u_g_m;
		}
		static const eastl::string& gc_table() {
			static const eastl::string g_t = eastl::string("sol.").append(detail::demangle<T>()).append(".\xE2\x99\xBB");
			return g_t;
		}
	};

} // namespace sol

#endif // SOL_USERTYPE_TRAITS_HPP
