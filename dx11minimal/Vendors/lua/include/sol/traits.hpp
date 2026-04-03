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

#ifndef SOL_TRAITS_HPP
#define SOL_TRAITS_HPP

#include <sol/tuple.hpp>
#include <sol/bind_traits.hpp>
#include <sol/pointer_like.hpp>
#include <sol/base_traits.hpp>
#include <sol/string_view.hpp>

#include <EASTL/type_traits.h>
#include <cstdint>
#include <EASTL/memory.h>
#include <EASTL/functional.h>
#include <EASTL/array.h>
#include <EASTL/iterator.h>
#include <iosfwd>
#if SOL_IS_ON(SOL_STD_VARIANT)
#include <EASTL/variant.h>
#endif // variant is weird on XCode, thanks XCode


namespace sol { namespace meta {
	template <typename T>
	struct unwrapped {
		typedef T type;
	};

	template <typename T>
	struct unwrapped<eastl::reference_wrapper<T>> {
		typedef T type;
	};

	template <typename T>
	using unwrapped_t = typename unwrapped<T>::type;

	template <typename T>
	struct unwrap_unqualified : unwrapped<unqualified_t<T>> { };

	template <typename T>
	using unwrap_unqualified_t = typename unwrap_unqualified<T>::type;

	template <typename T>
	struct remove_member_pointer;

	template <typename R, typename T>
	struct remove_member_pointer<R T::*> {
		typedef R type;
	};

	template <typename R, typename T>
	struct remove_member_pointer<R T::*const> {
		typedef R type;
	};

	template <typename T>
	using remove_member_pointer_t = remove_member_pointer<T>;

	template <typename T, typename...>
	struct all_same : eastl::true_type { };

	template <typename T, typename U, typename... Args>
	struct all_same<T, U, Args...> : eastl::integral_constant<bool, eastl::is_same<T, U>::value && all_same<T, Args...>::value> { };

	template <typename T, typename...>
	struct any_same : eastl::false_type { };

	template <typename T, typename U, typename... Args>
	struct any_same<T, U, Args...> : eastl::integral_constant<bool, eastl::is_same<T, U>::value || any_same<T, Args...>::value> { };

	template <typename T, typename... Args>
	constexpr inline bool any_same_v = any_same<T, Args...>::value;

	template <bool B>
	using boolean = eastl::integral_constant<bool, B>;

	template <bool B>
	constexpr inline bool boolean_v = boolean<B>::value;

	template <typename T>
	using neg = boolean<!T::value>;

	template <typename T>
	constexpr inline bool neg_v = neg<T>::value;

	template <typename... Args>
	struct all : boolean<true> { };

	template <typename T, typename... Args>
	struct all<T, Args...> : eastl::conditional_t<T::value, all<Args...>, boolean<false>> { };

	template <typename... Args>
	struct any : boolean<false> { };

	template <typename T, typename... Args>
	struct any<T, Args...> : eastl::conditional_t<T::value, boolean<true>, any<Args...>> { };

	template <typename... Args>
	constexpr inline bool all_v = all<Args...>::value;

	template <typename... Args>
	constexpr inline bool any_v = any<Args...>::value;

	enum class enable_t { _ };

	constexpr const auto enabler = enable_t::_;

	template <bool value, typename T = void>
	using disable_if_t = eastl::enable_if_t<!value, T>;

	template <typename... Args>
	using enable = eastl::enable_if_t<all<Args...>::value, enable_t>;

	template <typename... Args>
	using disable = eastl::enable_if_t<neg<all<Args...>>::value, enable_t>;

	template <typename... Args>
	using enable_any = eastl::enable_if_t<any<Args...>::value, enable_t>;

	template <typename... Args>
	using disable_any = eastl::enable_if_t<neg<any<Args...>>::value, enable_t>;

	template <typename V, typename... Vs>
	struct find_in_pack_v : boolean<false> { };

	template <typename V, typename Vs1, typename... Vs>
	struct find_in_pack_v<V, Vs1, Vs...> : any<boolean<(V::value == Vs1::value)>, find_in_pack_v<V, Vs...>> { };

	namespace meta_detail {
		template <eastl::size_t I, typename T, typename... Args>
		struct index_in_pack : eastl::integral_constant<eastl::size_t, SIZE_MAX> { };

		template <eastl::size_t I, typename T, typename T1, typename... Args>
		struct index_in_pack<I, T, T1, Args...>
		: conditional_t<eastl::is_same<T, T1>::value, eastl::integral_constant<std::ptrdiff_t, I>, index_in_pack<I + 1, T, Args...>> { };
	} // namespace meta_detail

	template <typename T, typename... Args>
	struct index_in_pack : meta_detail::index_in_pack<0, T, Args...> { };

	template <typename T, typename List>
	struct index_in : meta_detail::index_in_pack<0, T, List> { };

	template <typename T, typename... Args>
	struct index_in<T, types<Args...>> : meta_detail::index_in_pack<0, T, Args...> { };

	template <eastl::size_t I, typename... Args>
	struct at_in_pack { };

	template <eastl::size_t I, typename... Args>
	using at_in_pack_t = typename at_in_pack<I, Args...>::type;

	template <eastl::size_t I, typename Arg, typename... Args>
	struct at_in_pack<I, Arg, Args...> : eastl::conditional<I == 0, Arg, at_in_pack_t<I - 1, Args...>> { };

	template <typename Arg, typename... Args>
	struct at_in_pack<0, Arg, Args...> {
		typedef Arg type;
	};

	namespace meta_detail {
		template <typename, typename TI>
		using on_even = meta::boolean<(TI::value % 2) == 0>;

		template <typename, typename TI>
		using on_odd = meta::boolean<(TI::value % 2) == 1>;

		template <typename, typename>
		using on_always = eastl::true_type;

		template <template <typename...> class When, eastl::size_t Limit, eastl::size_t I, template <typename...> class Pred, typename... Ts>
		struct count_when_for_pack : eastl::integral_constant<eastl::size_t, 0> { };
		template <template <typename...> class When, eastl::size_t Limit, eastl::size_t I, template <typename...> class Pred, typename T, typename... Ts>
			          struct count_when_for_pack<When, Limit, I, Pred, T, Ts...> : conditional_t < sizeof...(Ts)
			     == 0
			|| Limit<2, eastl::integral_constant<eastl::size_t, I + static_cast<eastl::size_t>(Limit != 0 && Pred<T>::value)>,
			     count_when_for_pack<When, Limit - static_cast<eastl::size_t>(When<T, eastl::integral_constant<eastl::size_t, I>>::value),
			          I + static_cast<eastl::size_t>(When<T, eastl::integral_constant<eastl::size_t, I>>::value&& Pred<T>::value), Pred, Ts...>> { };
	} // namespace meta_detail

	template <template <typename...> class Pred, typename... Ts>
	struct count_for_pack : meta_detail::count_when_for_pack<meta_detail::on_always, sizeof...(Ts), 0, Pred, Ts...> { };

	template <template <typename...> class Pred, typename... Ts>
	inline constexpr eastl::size_t count_for_pack_v = count_for_pack<Pred, Ts...>::value;

	template <template <typename...> class Pred, typename List>
	struct count_for;

	template <template <typename...> class Pred, typename... Args>
	struct count_for<Pred, types<Args...>> : count_for_pack<Pred, Args...> { };

	template <eastl::size_t Limit, template <typename...> class Pred, typename... Ts>
	struct count_for_to_pack : meta_detail::count_when_for_pack<meta_detail::on_always, Limit, 0, Pred, Ts...> { };

	template <eastl::size_t Limit, template <typename...> class Pred, typename... Ts>
	inline constexpr eastl::size_t count_for_to_pack_v = count_for_to_pack<Limit, Pred, Ts...>::value;

	template <template <typename...> class When, eastl::size_t Limit, template <typename...> class Pred, typename... Ts>
	struct count_when_for_to_pack : meta_detail::count_when_for_pack<When, Limit, 0, Pred, Ts...> { };

	template <template <typename...> class When, eastl::size_t Limit, template <typename...> class Pred, typename... Ts>
	inline constexpr eastl::size_t count_when_for_to_pack_v = count_when_for_to_pack<When, Limit, Pred, Ts...>::value;

	template <template <typename...> class Pred, typename... Ts>
	using count_even_for_pack = count_when_for_to_pack<meta_detail::on_even, sizeof...(Ts), Pred, Ts...>;

	template <template <typename...> class Pred, typename... Ts>
	inline constexpr eastl::size_t count_even_for_pack_v = count_even_for_pack<Pred, Ts...>::value;

	template <template <typename...> class Pred, typename... Ts>
	using count_odd_for_pack = count_when_for_to_pack<meta_detail::on_odd, sizeof...(Ts), Pred, Ts...>;

	template <template <typename...> class Pred, typename... Ts>
	inline constexpr eastl::size_t count_odd_for_pack_v = count_odd_for_pack<Pred, Ts...>::value;

	template <typename... Args>
	struct return_type {
		typedef eastl::tuple<Args...> type;
	};

	template <typename T>
	struct return_type<T> {
		typedef T type;
	};

	template <>
	struct return_type<> {
		typedef void type;
	};

	template <typename... Args>
	using return_type_t = typename return_type<Args...>::type;

	namespace meta_detail {
		template <typename>
		struct always_true : eastl::true_type { };
		struct is_invokable_tester {
			template <typename Fun, typename... Args>
			static always_true<decltype(eastl::declval<Fun>()(eastl::declval<Args>()...))> test(int);
			template <typename...>
			static eastl::false_type test(...);
		};
	} // namespace meta_detail

	template <typename T>
	struct is_invokable;
	template <typename Fun, typename... Args>
	struct is_invokable<Fun(Args...)> : decltype(meta_detail::is_invokable_tester::test<Fun, Args...>(0)) { };

	namespace meta_detail {

		template <typename T, typename = void>
		struct is_invocable : eastl::is_function<eastl::remove_pointer_t<T>> { };

		template <typename T>
		struct is_invocable<T,
			eastl::enable_if_t<eastl::is_final<unqualified_t<T>>::value && eastl::is_class<unqualified_t<T>>::value
			     && eastl::is_same<decltype(void(&T::operator())), void>::value>> { };

		template <typename T>
		struct is_invocable<T,
			eastl::enable_if_t<!eastl::is_final<unqualified_t<T>>::value && eastl::is_class<unqualified_t<T>>::value
			     && eastl::is_destructible<unqualified_t<T>>::value>> {
			struct F {
				void operator()() {};
			};
			struct Derived : T, F { };
			template <typename U, U>
			struct Check;

			template <typename V>
			static sfinae_no_t test(Check<void (F::*)(), &V::operator()>*);

			template <typename>
			static sfinae_yes_t test(...);

			static constexpr bool value = eastl::is_same_v<decltype(test<Derived>(0)), sfinae_yes_t>;
		};

		template <typename T>
		struct is_invocable<T,
			eastl::enable_if_t<!eastl::is_final<unqualified_t<T>>::value && eastl::is_class<unqualified_t<T>>::value
			     && !eastl::is_destructible<unqualified_t<T>>::value>> {
			struct F {
				void operator()() {};
			};
			struct Derived : T, F {
				~Derived() = delete;
			};
			template <typename U, U>
			struct Check;

			template <typename V>
			static sfinae_no_t test(Check<void (F::*)(), &V::operator()>*);

			template <typename>
			static sfinae_yes_t test(...);

			static constexpr bool value = eastl::is_same_v<decltype(test<Derived>(0)), sfinae_yes_t>;
		};

		struct has_begin_end_impl {
			template <typename T, typename U = unqualified_t<T>, typename B = decltype(eastl::declval<U&>().begin()),
				typename E = decltype(eastl::declval<U&>().end())>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_key_type_impl {
			template <typename T, typename U = unqualified_t<T>, typename V = typename U::key_type>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_key_comp_impl {
			template <typename T, typename V = decltype(eastl::declval<unqualified_t<T>>().key_comp())>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_load_factor_impl {
			template <typename T, typename V = decltype(eastl::declval<unqualified_t<T>>().load_factor())>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_mapped_type_impl {
			template <typename T, typename V = typename unqualified_t<T>::mapped_type>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_value_type_impl {
			template <typename T, typename V = typename unqualified_t<T>::value_type>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_iterator_impl {
			template <typename T, typename V = typename unqualified_t<T>::iterator>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		struct has_key_value_pair_impl {
			template <typename T, typename U = unqualified_t<T>, typename V = typename U::value_type, typename F = decltype(eastl::declval<V&>().first),
				typename S = decltype(eastl::declval<V&>().second)>
			static eastl::true_type test(int);

			template <typename...>
			static eastl::false_type test(...);
		};

		template <typename T>
		struct has_push_back_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().push_back(eastl::declval<eastl::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = eastl::is_same_v<decltype(test<T>(0)), sfinae_yes_t>;
		};

		template <typename T>
		struct has_insert_with_iterator_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().insert(
				eastl::declval<eastl::add_rvalue_reference_t<typename C::iterator>>(), eastl::declval<eastl::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = !eastl::is_same_v<decltype(test<T>(0)), sfinae_no_t>;
		};

		template <typename T>
		struct has_insert_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().insert(eastl::declval<eastl::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = !eastl::is_same_v<decltype(test<T>(0)), sfinae_no_t>;
		};

		template <typename T>
		struct has_insert_after_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().insert_after(eastl::declval<eastl::add_rvalue_reference_t<typename C::const_iterator>>(),
				eastl::declval<eastl::add_rvalue_reference_t<typename C::value_type>>()))*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = eastl::is_same_v<decltype(test<T>(0)), sfinae_yes_t>;
		};

		template <typename T>
		struct has_size_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().size())*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = eastl::is_same_v<decltype(test<T>(0)), sfinae_yes_t>;
		};

		template <typename T>
		struct has_max_size_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().max_size())*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = eastl::is_same_v<decltype(test<T>(0)), sfinae_yes_t>;
		};

		template <typename T>
		struct has_to_string_test {
		private:
			template <typename C>
			static sfinae_yes_t test(decltype(eastl::declval<C>().to_string())*);
			template <typename C>
			static sfinae_no_t test(...);

		public:
			static constexpr bool value = eastl::is_same_v<decltype(test<T>(0)), sfinae_yes_t>;
		};

		template <typename T, typename U, typename = void>
		class supports_op_less_test : public eastl::false_type { };
		template <typename T, typename U>
		class supports_op_less_test<T, U, void_t<decltype(eastl::declval<T&>() < eastl::declval<U&>())>>
		: public eastl::integral_constant<bool,
#if SOL_IS_ON(SOL_STD_VARIANT)
			  !is_specialization_of_v<unqualified_t<T>, eastl::variant> && !is_specialization_of_v<unqualified_t<U>, eastl::variant>
#else
			  true
#endif
			  > {
		};

		template <typename T, typename U, typename = void>
		class supports_op_equal_test : public eastl::false_type { };
		template <typename T, typename U>
		class supports_op_equal_test<T, U, void_t<decltype(eastl::declval<T&>() == eastl::declval<U&>())>>
		: public eastl::integral_constant<bool,
#if SOL_IS_ON(SOL_STD_VARIANT)
			  !is_specialization_of_v<unqualified_t<T>, eastl::variant> && !is_specialization_of_v<unqualified_t<U>, eastl::variant>
#else
			  true
#endif
			  > {
		};

		template <typename T, typename U, typename = void>
		class supports_op_less_equal_test : public eastl::false_type { };
		template <typename T, typename U>
		class supports_op_less_equal_test<T, U, void_t<decltype(eastl::declval<T&>() <= eastl::declval<U&>())>>
		: public eastl::integral_constant<bool,
#if SOL_IS_ON(SOL_STD_VARIANT)
			  !is_specialization_of_v<unqualified_t<T>, eastl::variant> && !is_specialization_of_v<unqualified_t<U>, eastl::variant>
#else
			  true
#endif
			  > {
		};

		template <typename T, typename U, typename = void>
		class supports_op_left_shift_test : public eastl::false_type { };
		template <typename T, typename U>
		class supports_op_left_shift_test<T, U, void_t<decltype(eastl::declval<T&>() << eastl::declval<U&>())>> : public eastl::true_type { };

		template <typename T, typename = void>
		class supports_adl_to_string_test : public eastl::false_type { };
		template <typename T>
		class supports_adl_to_string_test<T, void_t<decltype(to_string(eastl::declval<const T&>()))>> : public eastl::true_type { };

		template <typename T, bool b>
		struct is_matched_lookup_impl : eastl::false_type { };
		template <typename T>
		struct is_matched_lookup_impl<T, true> : eastl::is_same<typename T::key_type, typename T::value_type> { };

		template <typename T>
		using non_void_t = meta::conditional_t<eastl::is_void_v<T>, ::sol::detail::unchecked_t, T>;

		template <typename T>
		using detect_sentinel = typename T::sentinel;
	} // namespace meta_detail

	template <typename T, typename Fallback>
	class sentinel_or {
	public:
		using type = detected_or_t<Fallback, meta_detail::detect_sentinel, T>;
	};

	template <typename T, typename Fallback>
	using sentinel_or_t = typename sentinel_or<T, Fallback>::type;

	template <typename T, typename U = T>
	class supports_op_less : public meta_detail::supports_op_less_test<T, U> { };

	template <typename T, typename U = T>
	class supports_op_equal : public meta_detail::supports_op_equal_test<T, U> { };

	template <typename T, typename U = T>
	class supports_op_less_equal : public meta_detail::supports_op_less_equal_test<T, U> { };

	template <typename T, typename U = T>
	class supports_op_left_shift : public meta_detail::supports_op_left_shift_test<T, U> { };

	template <typename T>
	class supports_adl_to_string : public meta_detail::supports_adl_to_string_test<T> { };

	template <typename T>
	class supports_to_string_member : public meta::boolean<meta_detail::has_to_string_test<meta_detail::non_void_t<T>>::value> { };

	template <typename T>
	using is_invocable = boolean<meta_detail::is_invocable<T>::value>;

	template <typename T>
	constexpr inline bool is_invocable_v = is_invocable<T>::value;

	template <typename T>
	struct has_begin_end : decltype(meta_detail::has_begin_end_impl::test<T>(0)) { };

	template <typename T>
	constexpr inline bool has_begin_end_v = has_begin_end<T>::value;

	template <typename T>
	struct has_key_value_pair : decltype(meta_detail::has_key_value_pair_impl::test<T>(0)) { };

	template <typename T>
	struct has_key_type : decltype(meta_detail::has_key_type_impl::test<T>(0)) { };

	template <typename T>
	struct has_key_comp : decltype(meta_detail::has_key_comp_impl::test<T>(0)) { };

	template <typename T>
	struct has_load_factor : decltype(meta_detail::has_load_factor_impl::test<T>(0)) { };

	template <typename T>
	struct has_mapped_type : decltype(meta_detail::has_mapped_type_impl::test<T>(0)) { };

	template <typename T>
	struct has_iterator : decltype(meta_detail::has_iterator_impl::test<T>(0)) { };

	template <typename T>
	struct has_value_type : decltype(meta_detail::has_value_type_impl::test<T>(0)) { };

	template <typename T>
	using has_push_back = meta::boolean<meta_detail::has_push_back_test<T>::value>;

	template <typename T>
	using has_max_size = meta::boolean<meta_detail::has_max_size_test<T>::value>;

	template <typename T>
	using has_insert = meta::boolean<meta_detail::has_insert_test<T>::value>;

	template <typename T>
	using has_insert_with_iterator = meta::boolean<meta_detail::has_insert_with_iterator_test<T>::value>;

	template <typename T>
	using has_insert_after = meta::boolean<meta_detail::has_insert_after_test<T>::value>;

	template <typename T>
	using has_size = meta::boolean<meta_detail::has_size_test<T>::value>;

	template <typename T>
	using is_associative = meta::all<has_key_type<T>, has_key_value_pair<T>, has_mapped_type<T>>;

	template <typename T>
	using is_lookup = meta::all<has_key_type<T>, has_value_type<T>>;

	template <typename T>
	using is_ordered = meta::all<has_key_comp<T>, meta::neg<has_load_factor<T>>>;

	template <typename T>
	using is_matched_lookup = meta_detail::is_matched_lookup_impl<T, is_lookup<T>::value>;

	template <typename T>
	using is_initializer_list = meta::is_specialization_of<T, std::initializer_list>;

	template <typename T>
	constexpr inline bool is_initializer_list_v = is_initializer_list<T>::value;

	template <typename T, typename CharT = char>
	using is_string_literal_array_of = boolean<eastl::is_array_v<T> && eastl::is_same_v<eastl::remove_all_extents_t<T>, CharT>>;

	template <typename T, typename CharT = char>
	constexpr inline bool is_string_literal_array_of_v = is_string_literal_array_of<T, CharT>::value;

	template <typename T>
	using is_string_literal_array = boolean<eastl::is_array_v<T>
		&& any_same_v<eastl::remove_all_extents_t<T>, char,
#if SOL_IS_ON(SOL_CHAR8_T)
		     char8_t,
#endif
		     char16_t, char32_t, wchar_t>>;

	template <typename T>
	constexpr inline bool is_string_literal_array_v = is_string_literal_array<T>::value;

	template <typename T, typename CharT>
	struct is_string_of : eastl::false_type { };

	template <typename CharT, typename CharTargetT, typename AllocT>
	struct is_string_of<eastl::basic_string<CharT, AllocT>, CharTargetT> : eastl::is_same<CharT, CharTargetT> { };

	template <typename T, typename CharT>
	constexpr inline bool is_string_of_v = is_string_of<T, CharT>::value;

	template <typename T, typename CharT>
	struct is_string_view_of : eastl::false_type { };

	template <typename CharT, typename CharTargetT>
	struct is_string_view_of<eastl::basic_string_view<CharT>, CharTargetT> : eastl::is_same<CharT, CharTargetT> { };

	template <typename T, typename CharT>
	constexpr inline bool is_string_view_of_v = is_string_view_of<T, CharT>::value;

	template <typename T>
	using is_string_like
		= meta::boolean<is_specialization_of_v<T, eastl::basic_string> || is_specialization_of_v<T, eastl::basic_string_view> || is_string_literal_array_v<T>>;

	template <typename T>
	constexpr inline bool is_string_like_v = is_string_like<T>::value;

	template <typename T, typename CharT = char>
	using is_string_constructible = meta::boolean<is_string_literal_array_of_v<T, CharT> || eastl::is_same_v<T, const CharT*> || eastl::is_same_v<T, CharT>
		|| is_string_of_v<T, CharT> || eastl::is_same_v<T, std::initializer_list<CharT>> || is_string_view_of_v<T, CharT> || eastl::is_null_pointer_v<T>>;

	template <typename T, typename CharT = char>
	constexpr inline bool is_string_constructible_v = is_string_constructible<T, CharT>::value;

	template <typename T>
	using is_string_like_or_constructible = meta::boolean<is_string_like_v<T> || is_string_constructible_v<T>>;

	template <typename T>
	struct is_pair : eastl::false_type { };

	template <typename T1, typename T2>
	struct is_pair<eastl::pair<T1, T2>> : eastl::true_type { };

	template <typename T, typename Char>
	using is_c_str_of = any<eastl::is_same<T, const Char*>, eastl::is_same<T, Char const* const>, eastl::is_same<T, Char*>, is_string_literal_array_of<T, Char>>;

	template <typename T, typename Char>
	constexpr inline bool is_c_str_of_v = is_c_str_of<T, Char>::value;

	template <typename T>
	using is_c_str = is_c_str_of<T, char>;

	template <typename T>
	constexpr inline bool is_c_str_v = is_c_str<T>::value;

	template <typename T, typename Char>
	using is_c_str_or_string_of = any<is_c_str_of<T, Char>, is_string_of<T, Char>>;

	template <typename T, typename Char>
	constexpr inline bool is_c_str_or_string_of_v = is_c_str_or_string_of<T, Char>::value;

	template <typename T>
	using is_c_str_or_string = is_c_str_or_string_of<T, char>;

	template <typename T>
	constexpr inline bool is_c_str_or_string_v = is_c_str_or_string<T>::value;

	template <typename T>
	struct is_move_only : all<neg<eastl::is_reference<T>>, neg<eastl::is_copy_constructible<unqualified_t<T>>>, eastl::is_move_constructible<unqualified_t<T>>> { };

	template <typename T>
	using is_not_move_only = neg<is_move_only<T>>;

	namespace meta_detail {
		template <typename T>
		decltype(auto) force_tuple(T&& x) {
			if constexpr (meta::is_specialization_of_v<meta::unqualified_t<T>, eastl::tuple>) {
				return eastl::forward<T>(x);
			}
			else {
				return eastl::tuple<T>(eastl::forward<T>(x));
			}
		}
	} // namespace meta_detail

	template <typename... X>
	decltype(auto) tuplefy(X&&... x)
	{
		return eastl::tuple_cat(meta_detail::force_tuple(eastl::forward<X>(x))...);
	}

	template <typename T, typename = void>
	struct iterator_tag {
		using type = eastl::input_iterator_tag;
	};

	template <typename T>
	struct iterator_tag<T, conditional_t<false, typename eastl::iterator_traits<T>::iterator_category, void>> {
		using type = typename eastl::iterator_traits<T>::iterator_category;
	};
}}     // namespace sol::meta

#endif // SOL_TRAITS_HPP
