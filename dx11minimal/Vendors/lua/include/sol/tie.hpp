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

#ifndef SOL_TIE_HPP
#define SOL_TIE_HPP

#include <sol/traits.hpp>

namespace sol {

	namespace detail {
		template <typename T>
		struct is_speshul : eastl::false_type { };
	} // namespace detail

	template <typename T>
	struct tie_size : eastl::tuple_size<T> { };

	template <typename T>
	struct is_tieable : eastl::integral_constant<bool, (::sol::tie_size<T>::value > 0)> { };

	template <typename... Tn>
	struct tie_t : public eastl::tuple<eastl::add_lvalue_reference_t<Tn>...> {
	private:
		typedef eastl::tuple<eastl::add_lvalue_reference_t<Tn>...> base_t;

		template <typename T>
		void set(eastl::false_type, T&& target) {
			eastl::get<0>(*this) = eastl::forward<T>(target);
		}

		template <typename T>
		void set(eastl::true_type, T&& target) {
			typedef tie_size<meta::unqualified_t<T>> value_size;
			typedef tie_size<eastl::tuple<Tn...>> tie_size;
			typedef meta::conditional_t<(value_size::value < tie_size::value), value_size, tie_size> indices_size;
			typedef eastl::make_index_sequence<indices_size::value> indices;
			set_extra(detail::is_speshul<meta::unqualified_t<T>>(), indices(), eastl::forward<T>(target));
		}

		template <eastl::size_t... I, typename T>
		void set_extra(eastl::true_type, eastl::index_sequence<I...>, T&& target) {
			using eastl::get;
			(void)detail::swallow { 0, (get<I>(static_cast<base_t&>(*this)) = get<I>(types<Tn...>(), target), 0)..., 0 };
		}

		template <eastl::size_t... I, typename T>
		void set_extra(eastl::false_type, eastl::index_sequence<I...>, T&& target) {
			using eastl::get;
			(void)detail::swallow { 0, (get<I>(static_cast<base_t&>(*this)) = get<I>(target), 0)..., 0 };
		}

	public:
		using base_t::base_t;

		template <typename T>
		tie_t& operator=(T&& value) {
			typedef is_tieable<meta::unqualified_t<T>> tieable;
			set(tieable(), eastl::forward<T>(value));
			return *this;
		}
	};

	template <typename... Tn>
	struct tie_size<tie_t<Tn...>> : eastl::tuple_size<eastl::tuple<Tn...>> { };

	namespace adl_barrier_detail {
		template <typename... Tn>
		inline tie_t<eastl::remove_reference_t<Tn>...> tie(Tn&&... argn) {
			return tie_t<eastl::remove_reference_t<Tn>...>(eastl::forward<Tn>(argn)...);
		}
	} // namespace adl_barrier_detail

	using namespace adl_barrier_detail;

} // namespace sol

#endif // SOL_TIE_HPP
