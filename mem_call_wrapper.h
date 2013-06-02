/*-
 * Copyright (c) 2012, 2013 Zhihao Yuan.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _MEM_CALL_WRAPPER_H
#define _MEM_CALL_WRAPPER_H 1

#include <functional>

namespace stdex {

template <typename T, typename Mfp>
struct mem_call_wrapper;

template <typename T, typename Mfp>
inline auto ref(T& t, Mfp f) noexcept
	-> mem_call_wrapper<T, Mfp> {
	return { t, f };
}

template <typename T, typename Mfp>
inline auto ref(mem_call_wrapper<T, Mfp> t) noexcept
	-> mem_call_wrapper<T, Mfp> {
	return { t.get_object(), t.get_pointer() };
}

template <typename T, typename Mfp>
inline auto cref(T const& t, Mfp f) noexcept
	-> mem_call_wrapper<T const, Mfp> {
	return { t, f };
}

template <typename T, typename Mfp>
inline auto cref(mem_call_wrapper<T, Mfp> t) noexcept
	-> mem_call_wrapper<T const, Mfp> {
	return { t.get_object(), t.get_pointer() };
}

template <typename T>
inline auto ref(T& t) noexcept
	-> std::reference_wrapper<T> {
	return t;
}

#if 0
template <typename T>
inline auto ref(std::reference_wrapper<T> t) noexcept
	-> std::reference_wrapper<T> {
	return t.get();
}
#endif

template <typename T>
inline auto cref(T const& t) noexcept
	-> std::reference_wrapper<T const> {
	return t;
}

#if 0
template <typename T>
inline auto cref(std::reference_wrapper<T> t) noexcept
	-> std::reference_wrapper<T const> {
	return t.get();
}
#endif

template <typename T> void ref(T const&&) = delete;
template <typename T> void cref(T const&&) = delete;

template <typename T, typename Mfp> void ref(T const&&, Mfp) = delete;
template <typename T, typename Mfp> void cref(T const&&, Mfp) = delete;

template <typename>
struct _mem_call_typedefs_impl;	// invalidate other mem_call_wrapper

template <typename T>
struct _mem_call_typedefs
	: _mem_call_typedefs_impl<typename std::remove_cv<T>::type>
{};

template <typename R, class C>
struct _mem_call_typedefs_impl<R(C::*)()> {
	typedef R	result_type;
};

template <typename R, class C, typename A1>
struct _mem_call_typedefs_impl<R(C::*)(A1)>
	: std::unary_function<A1, R>
{};

template <typename R, class C, typename A1, typename A2>
struct _mem_call_typedefs_impl<R(C::*)(A1, A2)>
	: std::binary_function<A1, A2, R>
{};

template <typename R, class C, typename A1, typename A2, typename... A3>
struct _mem_call_typedefs_impl<R(C::*)(A1, A2, A3...)> {
	typedef R	result_type;
};

template <typename T, typename Mfp>
struct mem_call_wrapper : _mem_call_typedefs<Mfp> {
	typedef T	object_type;
	typedef Mfp	pointer_type;

	mem_call_wrapper(object_type& t, pointer_type f) : t_(&t), f_(f) {}
	mem_call_wrapper(object_type&& t, pointer_type f) = delete;

	object_type& get_object() const noexcept {
		return *t_;
	}

	pointer_type get_pointer() const noexcept {
		return f_;
	}

	template <typename... Args>
	auto operator()(Args&&... args) const
		-> typename std::result_of<Mfp(T&, Args...)>::type {
		return (t_->*f_)(std::forward<Args>(args)...);
	}

private:
	object_type	*t_;
	pointer_type	 f_;
};

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator==(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return x.get_pointer() == y.get_pointer() and
		x.get_object() == y.get_object();
}

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator!=(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return !(x == y);
}

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator<(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return x.get_pointer() < y.get_pointer() ||
		(!(y.get_object() < x.get_object()) &&
		 x.get_pointer() < y.get_pointer());
}

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator>(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return y < x;
}

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator<=(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return !(y < x);
}

template <typename T1, typename Mfp1, typename T2, typename Mfp2>
inline bool operator>=(mem_call_wrapper<T1, Mfp1> const& x,
    mem_call_wrapper<T2, Mfp2> const& y) {
	return !(x < y);
}

}

#endif
