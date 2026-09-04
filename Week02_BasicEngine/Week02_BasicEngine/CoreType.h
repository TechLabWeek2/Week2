#pragma once
#include <iostream>

//~ Unsigned base types

// 8-bit unsigned integer
typedef unsigned char 		uint8;

// 16-bit unsigned integer
typedef unsigned short int	uint16;

// 32-bit unsigned integer
typedef unsigned int		uint32;

// 64-bit unsigned integer
typedef unsigned long long	uint64;

//~ Signed base types.

// 8-bit signed integer
typedef	signed char			int8;

// 16-bit signed integer
typedef signed short int	int16;

// 32-bit signed integer
typedef signed int	 		int32;

// 64-bit signed integer
typedef signed long long	int64;

//~ Character types

// An ANSI character. 8-bit fixed-width representation of 7-bit characters.
typedef char				ANSICHAR;

// A wide character. In-memory only. ?-bit fixed-width representation of the platform's natural wide character set. Could be different sizes on different platforms.
typedef wchar_t				WIDECHAR;

// An 8-bit character type. In-memory only. 8-bit representation.
using UTF8CHAR = char8_t;

// A 32-bit character type. In-memory only. 32-bit representation.
using UTF32CHAR = char32_t;

