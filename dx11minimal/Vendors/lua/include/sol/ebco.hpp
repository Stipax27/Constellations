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

#ifndef SOL_EBCO_HPP
#define SOL_EBCO_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include <EASTL/memory.h>

namespace sol { namespace detail {

	template <typename T, eastl::size_t tag = 0, typename = void>
	struct ebco {
		T m_value;

		ebco() = default;
		ebco(const ebco&) = default;
		ebco(ebco&&) = default;
		ebco& operator=(const ebco&) = default;
		ebco& operator=(ebco&&) = default;
		ebco(const T& v) noexcept(eastl::is_nothrow_copy_constructible_v<T>) : m_value(v) {};
		ebco(T&& v) noexcept(eastl::is_nothrow_move_constructible_v<T>) : m_value(eastl::move(v)) {};
		ebco& operator=(const T& v) noexcept(eastl::is_nothrow_copy_assignable_v<T>) {
			m_value = v;
			return *this;
		}
		ebco& operator=(T&& v) noexcept(eastl::is_nothrow_move_assignable_v<T>) {
			m_value = eastl::move(v);
			return *this;
		};
		template <typename Arg, typename... Args,
			typename = eastl::enable_if_t<
			     !eastl::is_same_v<eastl::remove_reference_t<eastl::remove_cv_t<Arg>>,
			          ebco> && !eastl::is_same_v<eastl::remove_reference_t<eastl::remove_cv_t<Arg>>, T> && (sizeof...(Args) > 0 || !eastl::is_convertible_v<Arg, T>)>>
		ebco(Arg&& arg, Args&&... args) noexcept(eastl::is_nothrow_constructible_v<T, Arg, Args...>)
		: m_value(eastl::forward<Arg>(arg), eastl::forward<Args>(args)...) {
		}

		T& value() & noexcept {
			return m_value;
		}

		T const& value() const& noexcept {
			return m_value;
		}

		T&& value() && noexcept {
			return eastl::move(m_value);
		}
	};

	template <typename T, eastl::size_t tag>
	struct ebco<T, tag, eastl::enable_if_t<!eastl::is_reference_v<T> && eastl::is_class_v<T> && !eastl::is_final_v<T>>> : T {
		ebco() = default;
		ebco(const ebco&) = default;
		ebco(ebco&&) = default;
		ebco(const T& v) noexcept(eastl::is_nothrow_copy_constructible_v<T>) : T(v) {};
		ebco(T&& v) noexcept(eastl::is_nothrow_move_constructible_v<T>) : T(eastl::move(v)) {};
		template <typename Arg, typename... Args,
			typename = eastl::enable_if_t<
			     !eastl::is_same_v<eastl::remove_reference_t<eastl::remove_cv_t<Arg>>,
			          ebco> && !eastl::is_same_v<eastl::remove_reference_t<eastl::remove_cv_t<Arg>>, T> && (sizeof...(Args) > 0 || !eastl::is_convertible_v<Arg, T>)>>
		ebco(Arg&& arg, Args&&... args) noexcept(eastl::is_nothrow_constructible_v<T, Arg, Args...>) : T(eastl::forward<Arg>(arg), eastl::forward<Args>(args)...) {
		}

		ebco& operator=(const ebco&) = default;
		ebco& operator=(ebco&&) = default;
		ebco& operator=(const T& v) noexcept(eastl::is_nothrow_copy_assignable_v<T>) {
			static_cast<T&>(*this) = v;
			return *this;
		}
		ebco& operator=(T&& v) noexcept(eastl::is_nothrow_move_assignable_v<T>) {
			static_cast<T&>(*this) = eastl::move(v);
			return *this;
		};

		T& value() & noexcept {
			return static_cast<T&>(*this);
		}

		T const& value() const& noexcept {
			return static_cast<T const&>(*this);
		}

		T&& value() && noexcept {
			return eastl::move(static_cast<T&>(*this));
		}
	};

	template <typename T, eastl::size_t tag>
	struct ebco<T&, tag> {
	private:
		T* m_ref;

	public:
		ebco() = default;
		ebco(const ebco&) = default;
		ebco(ebco&&) = default;
		ebco(T& v) noexcept : m_ref(eastl::addressof(v)) {};

		ebco& operator=(const ebco&) = default;
		ebco& operator=(ebco&&) = default;
		ebco& operator=(T& v) noexcept {
			m_ref = eastl::addressof(v);
			return *this;
		}

		T& value() const noexcept {
			return *(const_cast<ebco<T&, tag>&>(*this).m_ref);
		}
	};

	template <typename T, eastl::size_t tag>
	struct ebco<T&&, tag> {
		T&& ref;

		ebco() = default;
		ebco(const ebco&) = delete;
		ebco(ebco&&) = default;
		ebco(T&& v) noexcept : ref(v) {};

		ebco& operator=(const ebco&) = delete;
		ebco& operator=(ebco&&) = delete;

		T& value() & noexcept {
			return ref;
		}

		const T& value() const& noexcept {
			return ref;
		}

		T&& value() && noexcept {
			return eastl::move(ref);
		}
	};

}} // namespace sol::detail

#endif // SOL_EBCO_HPP
