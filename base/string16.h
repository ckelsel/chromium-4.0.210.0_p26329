/* Copyright 2016 kunming.xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BASE_STRING16_H_
#define BASE_STRING16_H_

// WHAT:
// A version of std::basic_string that provides 2-byte characters even when
// wchar_t is not implemented as a 2-byte type. You can access this class as
// string16. We also define char16, which string16 is based upon.
//
// WHY:
// On Windows, wchar_t is 2 bytes, and it can conveniently handle UTF-16/UCS-2
// data. Plenty of existing code operates on strings encoded as UTF-16.
//
// On many other platforms, sizeof(wchar_t) is 4 bytes by default. We can make
// it 2 bytes by using the GCC flag -fshort-wchar. But then std::wstring fails
// at run time, because it calls some functions (like wcslen) that come from
// the system's native C library -- which was built with a 4-byte wchar_t!
// It's wasteful to use 4-byte wchar_t strings to carry UTF-16 data, and it's
// entirely improper on those systems where the encoding of wchar_t is defined
// as UTF-32.
//
// Here, we define string16, which is similar to std::wstring but replaces all
// libc functions with custom, 2-byte-char compatible routines. It is capable
// of carrying UTF-16-encoded data.

#include <stdio.h>
#include <string>

#include "base/basictypes.h"

#if defined(WCHAR_T_IS_UTF16)

typedef wchar_t char16;
typedef std::wstring string16;

#elif defined(WCHAR_TIS_UTF32)
#error WCHAR_TIS_UTF32

typedef uint16 char16;

namespace base {
int c16memcmp(const char16 *s1, const char16 *s2, size_t n);
size_t c16len(const char16 *s);
const char16 *c16memchr(const char16 *s, char16 c, size_t n);
char16 *c16memmove(char16 *dest, const char16 *src, size_t n);
char16 *c16memcpy(char16 *dest, const char16 *src, size_t n);
char16 *c16memset(char16 *s, char16 c, size_t n);
};

#endif // WCHAR_TIS_UTF32

#endif // BASE_STRING16_H_
