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
#ifndef BASE_PORT_H_
#define BASE_PORT_H_

#include "build/build_config.h"

#ifdef COMPILER_MSVC
#define GG_LONGLONG(x)  x##I64
#define GG_ULONGLONG(x) x##UI64
#else
#define GG_LONGLONG(x)  x##LL
#define GG_ULONGLONG(x) x##ULL
#endif

#endif // BASE_PORT_H_
