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

#ifndef SOL_STACK_FIELD_HPP
#define SOL_STACK_FIELD_HPP

#include <sol/stack_core.hpp>
#include <sol/stack_push.hpp>
#include <sol/stack_get.hpp>
#include <sol/stack_check_get.hpp>

namespace sol { namespace stack {

	namespace stack_detail {
		template <typename T, bool global, bool raw>
		inline constexpr bool is_get_direct_tableless_v = (global && !raw && meta::is_c_str_or_string_v<T>);

		template <typename T, bool global, bool raw>
		inline constexpr bool is_get_direct_v = (is_get_direct_tableless_v<T, global, raw>) // cf-hack
			|| (!global && !raw && (meta::is_c_str_or_string_v<T> || meta::is_string_of_v<T, char>)) // cf-hack
			|| (!global && raw && (eastl::is_integral_v<T> && !eastl::is_same_v<T, bool>))
#if SOL_LUA_VERSION_I_ >= 503
			|| (!global && !raw && (eastl::is_integral_v<T> && !eastl::is_same_v<T, bool>))
#endif // integer keys 5.3 or better
#if SOL_LUA_VERSION_I_ >= 502
			|| (!global && raw && eastl::is_pointer_v<T> && eastl::is_void_v<eastl::remove_pointer_t<T>>)
#endif // void pointer keys 5.2 or better
			;

		template <typename T, bool global, bool raw>
		inline constexpr bool is_set_direct_tableless_v = (global && !raw && meta::is_c_str_or_string_v<T>);

		template <typename T, bool global, bool raw>
		inline constexpr bool is_set_direct_v = (is_set_direct_tableless_v<T, global, raw>) // cf-hack
			|| (!global && !raw && (meta::is_c_str_or_string_v<T> || meta::is_string_of_v<T, char>)) // cf-hack
			|| (!global && raw && (eastl::is_integral_v<T> && !eastl::is_same_v<T, bool>))     // cf-hack
#if SOL_LUA_VERSION_I_ >= 503
			|| (!global && !raw && (eastl::is_integral_v<T> && !eastl::is_same_v<T, bool>))
#endif // integer keys 5.3 or better
#if SOL_LUA_VERSION_I_ >= 502
			|| (!global && raw && (eastl::is_pointer_v<T> && eastl::is_void_v<eastl::remove_pointer_t<T>>))
#endif // void pointer keys 5.2 or better
			;
	} // namespace stack_detail

	template <typename T, bool global, bool raw, typename>
	struct field_getter {
		static inline constexpr int default_table_index
			= meta::conditional_t<stack_detail::is_get_direct_v<T, global, raw>, eastl::integral_constant<int, -1>, eastl::integral_constant<int, -2>>::value;

		template <typename Key>
		void get(lua_State* L, Key&& key, int tableindex = default_table_index) {
			if constexpr (eastl::is_same_v<T, update_if_empty_t> || eastl::is_same_v<T, override_value_t> || eastl::is_same_v<T, create_if_nil_t>) {
				(void)L;
				(void)key;
				(void)tableindex;
			}
			else if constexpr (eastl::is_same_v<T, env_key_t>) {
				(void)key;
#if SOL_LUA_VERSION_I_ < 502
				// Use lua_setfenv
				lua_getfenv(L, tableindex);
#else
				// Use upvalues as explained in Lua 5.2 and beyond's manual
				if (lua_getupvalue(L, tableindex, 1) == nullptr) {
					push(L, lua_nil);
				}
#endif
			}
			else if constexpr (eastl::is_same_v<T, metatable_key_t>) {
				(void)key;
				if (lua_getmetatable(L, tableindex) == 0)
					push(L, lua_nil);
			}
			else if constexpr (raw) {
				if constexpr (eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>) {
					lua_rawgeti(L, tableindex, static_cast<lua_Integer>(key));
				}
#if SOL_LUA_VERSION_I_ >= 502
				else if constexpr (eastl::is_pointer_v<T> && eastl::is_void_v<eastl::remove_pointer_t<T>>) {
					lua_rawgetp(L, tableindex, key);
				}
#endif // Lua 5.2.x+
				else {
					push(L, eastl::forward<Key>(key));
					lua_rawget(L, tableindex);
				}
			}
			else {
				if constexpr (meta::is_c_str_or_string_v<T>) {
					if constexpr (global) {
						(void)tableindex;
						lua_getglobal(L, &key[0]);
					}
					else {
						lua_getfield(L, tableindex, &key[0]);
					}
				}
				else if constexpr (eastl::is_same_v<T, meta_function>) {
					const auto& real_key = to_string(key);
					lua_getfield(L, tableindex, &real_key[0]);
				}
#if SOL_LUA_VERSION_I_ >= 503
				else if constexpr (eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>) {
					lua_geti(L, tableindex, static_cast<lua_Integer>(key));
				}
#endif // Lua 5.3.x+
				else {
					push(L, eastl::forward<Key>(key));
					lua_gettable(L, tableindex);
				}
			}
		}
	};

	template <typename... Args, bool b, bool raw, typename C>
	struct field_getter<eastl::tuple<Args...>, b, raw, C> {
		template <eastl::size_t... I, typename Keys>
		void apply(eastl::index_sequence<0, I...>, lua_State* L, Keys&& keys, int tableindex) {
			get_field<b, raw>(L, eastl::get<0>(eastl::forward<Keys>(keys)), tableindex);
			void(detail::swallow { (get_field<false, raw>(L, eastl::get<I>(eastl::forward<Keys>(keys))), 0)... });
			reference saved(L, -1);
			lua_pop(L, static_cast<int>(sizeof...(I)));
			saved.push();
		}

		template <typename Keys>
		void get(lua_State* L, Keys&& keys) {
			apply(eastl::make_index_sequence<sizeof...(Args)>(), L, eastl::forward<Keys>(keys), lua_absindex(L, -1));
		}

		template <typename Keys>
		void get(lua_State* L, Keys&& keys, int tableindex) {
			apply(eastl::make_index_sequence<sizeof...(Args)>(), L, eastl::forward<Keys>(keys), tableindex);
		}
	};

	template <typename A, typename B, bool b, bool raw, typename C>
	struct field_getter<eastl::pair<A, B>, b, raw, C> {
		template <typename Keys>
		void get(lua_State* L, Keys&& keys, int tableindex) {
			get_field<b, raw>(L, eastl::get<0>(eastl::forward<Keys>(keys)), tableindex);
			get_field<false, raw>(L, eastl::get<1>(eastl::forward<Keys>(keys)));
			reference saved(L, -1);
			lua_pop(L, static_cast<int>(2));
			saved.push();
		}

		template <typename Keys>
		void get(lua_State* L, Keys&& keys) {
			get_field<b, raw>(L, eastl::get<0>(eastl::forward<Keys>(keys)));
			get_field<false, raw>(L, eastl::get<1>(eastl::forward<Keys>(keys)));
			reference saved(L, -1);
			lua_pop(L, static_cast<int>(2));
			saved.push();
		}
	};

	template <typename T, bool global, bool raw, typename>
	struct field_setter {
		static constexpr int default_table_index
			= meta::conditional_t<stack_detail::is_set_direct_v<T, global, raw>, eastl::integral_constant<int, -2>, eastl::integral_constant<int, -3>>::value;

		template <typename Key, typename Value>
		void set(lua_State* L, Key&& key, Value&& value, int tableindex = default_table_index) {
			if constexpr (eastl::is_same_v<T, update_if_empty_t> || eastl::is_same_v<T, override_value_t>) {
				(void)L;
				(void)key;
				(void)value;
				(void)tableindex;
			}
			else if constexpr (eastl::is_same_v<T, metatable_key_t>) {
				(void)key;
				push(L, eastl::forward<Value>(value));
				lua_setmetatable(L, tableindex);
			}
			else if constexpr (raw) {
				if constexpr (eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>) {
					push(L, eastl::forward<Value>(value));
					lua_rawseti(L, tableindex, static_cast<lua_Integer>(key));
				}
#if SOL_LUA_VERSION_I_ >= 502
				else if constexpr (eastl::is_pointer_v<T> && eastl::is_void_v<eastl::remove_pointer_t<T>>) {
					push(L, eastl::forward<Value>(value));
					lua_rawsetp(L, tableindex, eastl::forward<Key>(key));
				}
#endif // Lua 5.2.x
				else {
					push(L, eastl::forward<Key>(key));
					push(L, eastl::forward<Value>(value));
					lua_rawset(L, tableindex);
				}
			}
			else {
				if constexpr (meta::is_c_str_or_string_v<T>) {
					if constexpr (global) {
						push(L, eastl::forward<Value>(value));
						lua_setglobal(L, &key[0]);
						(void)tableindex;
					}
					else {
						push(L, eastl::forward<Value>(value));
						lua_setfield(L, tableindex, &key[0]);
					}
				}
#if SOL_LUA_VERSION_I_ >= 503
				else if constexpr (eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>) {
					push(L, eastl::forward<Value>(value));
					lua_seti(L, tableindex, static_cast<lua_Integer>(key));
				}
#endif // Lua 5.3.x
				else {
					push(L, eastl::forward<Key>(key));
					push(L, eastl::forward<Value>(value));
					lua_settable(L, tableindex);
				}
			}
		}
	};

	template <typename... Args, bool b, bool raw, typename C>
	struct field_setter<eastl::tuple<Args...>, b, raw, C> {
		template <bool g, eastl::size_t I, typename Keys, typename Value>
		void apply(eastl::index_sequence<I>, lua_State* L, Keys&& keys, Value&& value, int tableindex) {
			I < 1 ? set_field<g, raw>(L, eastl::get<I>(eastl::forward<Keys>(keys)), eastl::forward<Value>(value), tableindex)
				 : set_field<g, raw>(L, eastl::get<I>(eastl::forward<Keys>(keys)), eastl::forward<Value>(value));
		}

		template <bool g, eastl::size_t I0, eastl::size_t I1, eastl::size_t... I, typename Keys, typename Value>
		void apply(eastl::index_sequence<I0, I1, I...>, lua_State* L, Keys&& keys, Value&& value, int tableindex) {
			I0 < 1 ? get_field<g, raw>(L, eastl::get<I0>(eastl::forward<Keys>(keys)), tableindex)
				  : get_field<g, raw>(L, eastl::get<I0>(eastl::forward<Keys>(keys)), -1);
			apply<false>(eastl::index_sequence<I1, I...>(), L, eastl::forward<Keys>(keys), eastl::forward<Value>(value), -1);
		}

		template <bool g, eastl::size_t I0, eastl::size_t... I, typename Keys, typename Value>
		void top_apply(eastl::index_sequence<I0, I...>, lua_State* L, Keys&& keys, Value&& value, int tableindex) {
			apply<g>(eastl::index_sequence<I0, I...>(), L, eastl::forward<Keys>(keys), eastl::forward<Value>(value), tableindex);
			lua_pop(L, static_cast<int>(sizeof...(I)));
		}

		template <typename Keys, typename Value>
		void set(lua_State* L, Keys&& keys, Value&& value, int tableindex = -3) {
			top_apply<b>(eastl::make_index_sequence<sizeof...(Args)>(), L, eastl::forward<Keys>(keys), eastl::forward<Value>(value), tableindex);
		}
	};

	template <typename A, typename B, bool b, bool raw, typename C>
	struct field_setter<eastl::pair<A, B>, b, raw, C> {
		template <typename Keys, typename Value>
		void set(lua_State* L, Keys&& keys, Value&& value, int tableindex = -1) {
			get_field<b, raw>(L, eastl::get<0>(eastl::forward<Keys>(keys)), tableindex);
			set_field<false, raw>(L, eastl::get<1>(eastl::forward<Keys>(keys)), eastl::forward<Value>(value), lua_gettop(L));
			lua_pop(L, 1);
		}
	};
}} // namespace sol::stack

#endif // SOL_STACK_FIELD_HPP
