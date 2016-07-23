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
#include "base/string16.h"

#if defined(WCHAR_T_IS_UTF16)

#error This file should not be used on 2-byte wchar_t systems

#elif defined(WCHAR_TIS_UTF32)

namespace base {

int c16memcmp(const char16 *s1, const char16 *s2, size_t n)
{
    return 0;
}

size_t c16len(const char16 *s)
{
    return 0;
}

const char16 *c16memchr(const char16 *s, char16 c, size_t n)
{
    return NULL;
}

char16 *c16memmove(char16 *dest, const char16 *src, size_t n)
{
    return NULL;
}

char16 *c16memcpy(char16 *dest, const char16 *src, size_t n)
{
    return NULL;
}

char16 *c16memset(char16 *s, char16 c, size_t n)
{
    return NULL;
}

};

#endif
