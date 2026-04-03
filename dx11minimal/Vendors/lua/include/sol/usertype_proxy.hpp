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

#ifndef SOL_USERTYPE_PROXY_HPP
#define SOL_USERTYPE_PROXY_HPP

#include <sol/traits.hpp>
#include <sol/function.hpp>
#include <sol/protected_function.hpp>
#include <sol/proxy_base.hpp>

namespace sol {
	template <typename Table, typename Key>
	struct usertype_proxy : public proxy_base<usertype_proxy<Table, Key>> {
	private:
		using key_type = detail::proxy_key_t<Key>;

		template <typename T, eastl::size_t... I>
		decltype(auto) tuple_get(eastl::index_sequence<I...>) const& {
			return tbl.template traverse_get<T>(eastl::get<I>(key)...);
		}

		template <typename T, eastl::size_t... I>
		decltype(auto) tuple_get(eastl::index_sequence<I...>) && {
			return tbl.template traverse_get<T>(eastl::get<I>(eastl::move(key))...);
		}

		template <eastl::size_t... I, typename T>
		void tuple_set(eastl::index_sequence<I...>, T&& value) & {
			if constexpr (sizeof...(I) > 1) {
				tbl.traverse_set(eastl::get<I>(key)..., eastl::forward<T>(value));
			}
			else {
				tbl.set(eastl::get<I>(key)..., eastl::forward<T>(value));
			}
		}

		template <eastl::size_t... I, typename T>
		void tuple_set(eastl::index_sequence<I...>, T&& value) && {
			if constexpr (sizeof...(I) > 1) {
				tbl.traverse_set(eastl::get<I>(eastl::move(key))..., eastl::forward<T>(value));
			}
			else {
				tbl.set(eastl::get<I>(eastl::move(key))..., eastl::forward<T>(value));
			}
		}

	public:
		Table tbl;
		key_type key;

		template <typename T>
		usertype_proxy(Table table, T&& k) : tbl(table), key(eastl::forward<T>(k)) {
		}

		template <typename T>
		usertype_proxy& set(T&& item) & {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			tuple_set(idx_seq(), eastl::forward<T>(item));
			return *this;
		}

		template <typename T>
		usertype_proxy&& set(T&& item) && {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			eastl::move(*this).tuple_set(idx_seq(), eastl::forward<T>(item));
			return eastl::move(*this);
		}

		template <typename T>
		usertype_proxy& operator=(T&& other) & {
			return set(eastl::forward<T>(other));
		}

		template <typename T>
		usertype_proxy&& operator=(T&& other) && {
			return eastl::move(*this).set(eastl::forward<T>(other));
		}

		template <typename T>
		usertype_proxy& operator=(std::initializer_list<T> other) & {
			return set(eastl::move(other));
		}

		template <typename T>
		usertype_proxy&& operator=(std::initializer_list<T> other) && {
			return eastl::move(*this).set(eastl::move(other));
		}

		template <typename T>
		decltype(auto) get() const& {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			return tuple_get<T>(idx_seq());
		}

		template <typename T>
		decltype(auto) get() && {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			return eastl::move(*this).template tuple_get<T>(idx_seq());
		}

		template <typename K>
		decltype(auto) operator[](K&& k) const& {
			auto keys = meta::tuplefy(key, eastl::forward<K>(k));
			return usertype_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename K>
		decltype(auto) operator[](K&& k) & {
			auto keys = meta::tuplefy(key, eastl::forward<K>(k));
			return usertype_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename K>
		decltype(auto) operator[](K&& k) && {
			auto keys = meta::tuplefy(eastl::move(key), eastl::forward<K>(k));
			return usertype_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename... Ret, typename... Args>
		decltype(auto) call(Args&&... args) {
#if !defined(__clang__) && defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 191200000
			// MSVC is ass sometimes
			return get<function>().call<Ret...>(eastl::forward<Args>(args)...);
#else
			return get<function>().template call<Ret...>(eastl::forward<Args>(args)...);
#endif
		}

		template <typename... Args>
		decltype(auto) operator()(Args&&... args) {
			return call<>(eastl::forward<Args>(args)...);
		}

		bool valid() const {
			auto pp = stack::push_pop(tbl);
			auto p = stack::probe_get_field<eastl::is_same<meta::unqualified_t<Table>, global_table>::value>(lua_state(), key, lua_gettop(lua_state()));
			lua_pop(lua_state(), p.levels);
			return p;
		}

		int push() const noexcept {
			return push(this->lua_state());
		}

		int push(lua_State* L) const noexcept {
			return get<reference>().push(L);
		}

		type get_type() const {
			type t = type::none;
			auto pp = stack::push_pop(tbl);
			auto p = stack::probe_get_field<eastl::is_same<meta::unqualified_t<Table>, global_table>::value>(lua_state(), key, lua_gettop(lua_state()));
			if (p) {
				t = type_of(lua_state(), -1);
			}
			lua_pop(lua_state(), p.levels);
			return t;
		}

		lua_State* lua_state() const {
			return tbl.lua_state();
		}
	};
} // namespace sol

#endif // SOL_USERTYPE_PROXY_HPP
