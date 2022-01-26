// Copyright (C) embedded ocean GmbH, Munich, Germany.
//
// This file is part of the Charconv Float Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef CHARCONV_FLOAT_CHARCONV_H
#define	CHARCONV_FLOAT_CHARCONV_H

#include <charconv>

#if defined __GLIBCXX__ && _GLIBCXX_RELEASE < 11 && __cplusplus >= 201703L

#if __cpp_lib_to_chars < 201611L && _GLIBCXX_HAVE_USELOCALE
#	undef __cpp_lib_to_chars
#	define __cpp_lib_to_chars 201611L
#endif

#include <bits/charconv-float-config.h>

namespace std _GLIBCXX_VISIBILITY(default)
{

inline namespace __chaconv_float_41f576ab_104c_442f_926d_69a25265c6cd
{

	auto to_chars(char *first, char *last, float value) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, float value, chars_format fmt) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, float value, chars_format fmt, int precision) noexcept -> to_chars_result;

	auto to_chars(char *first, char *last, double value) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, double value, chars_format fmt) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, double value, chars_format fmt, int precision) noexcept -> to_chars_result;

	auto to_chars(char *first, char *last, long double value) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, long double value, chars_format fmt) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, long double value, chars_format fmt, int precision) noexcept -> to_chars_result;

	#ifdef FLOAT128_TO_CHARS
	auto to_chars(char *first, char *last, __float128 value) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, __float128 value, chars_format fmt) noexcept -> to_chars_result;
	auto to_chars(char *first, char *last, __float128 value, chars_format fmt, int precision) noexcept -> to_chars_result;
	#endif

	#if CHARCONV_FLOAT_HAVE_FROM_CHAR
	auto from_chars(
		const char *first, const char *last, float &value, chars_format fmt = std::chars_format::general) noexcept
		-> from_chars_result;

	auto from_chars(
		const char *first, const char *last, double &value, chars_format fmt = std::chars_format::general) noexcept
		-> from_chars_result;
	#endif

} // namespace __chaconv_float_41f576ab_104c_442f_926d_69a25265c6cd

} // namespace std

#endif // defined __GLIBCXX__ && _GLIBCXX_RELEASE < 11 && __cplusplus >= 201703L

#endif // CHARCONV_FLOAT_CHARCONV_H
