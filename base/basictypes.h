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
#ifndef _BASE_BASICTYPES_H_
#define _BASE_BASICTYPES_H_

#include <limits.h>
#include <stddef.h>
#include <string.h>

#ifndef COMPILER_MSVC
// stdint.h is part of C99 but MSVC doesn't have it.
#include <stdint.h>
#endif

#include "base/port.h"

#if defined(COMPILER_MSVC)
#pragma warning (push)
#pragma warning (disable:4310)
#endif

//
// signed
//
typedef signed char     schar;

typedef signed char     int8;

typedef signed short    int16;

typedef signed int      int32;

typedef signed long long    int64;

//
// unsigned
//
typedef unsigned char   uchar;

typedef unsigned char   uint8;

typedef unsigned short  uint16;

typedef unsigned int    uint32;

typedef unsigned long long  uint64;

//
// Range
//
const uint8  kuint8max  = (( uint8) 0xFF);
const uint16 kuint16max = ((uint16) 0xFFFF);
const uint32 kuint32max = ((uint32) 0xFFFFFFFF);
const uint64 kuint64max = ((uint64) GG_UINT64_C(0xFFFFFFFFFFFFFFFF));

const  int8  kint8max   = ((  int8) 0x7F);
const  int8  kint8min   = ((  int8) 0x80);
const  int16 kint16max  = (( int16) 0x7FFF);
const  int16 kint16min  = (( int16) 0x8000);
const  int32 kint32max  = (( int32) 0x7FFFFFFF);
const  int32 kint32min  = (( int32) 0x80000000);
const  int64 kint64max  = (( int64) GG_INT64_C(0x7FFFFFFFFFFFFFFF));
const  int64 kint64min  = (( int64) GG_INT64_C(0x8000000000000000));

#if defined(COMPILER_MSVC)
#pragma warning (pop)
#endif

// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.

template <bool>
struct CompileAssert {
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]

// Implementation details of COMPILE_ASSERT:
//
// - COMPILE_ASSERT works by defining an array type that has -1
//   elements (and thus is invalid) when the expression is false.
//
// - The simpler definition
//
//     #define COMPILE_ASSERT(expr, msg) typedef char msg[(expr) ? 1 : -1]
//
//   does not work, as gcc supports variable-length arrays whose sizes
//   are determined at run-time (this is gcc's extension and not part
//   of the C++ standard).  As a result, gcc fails to reject the
//   following code with the simple definition:
//
//     int foo;
//     COMPILE_ASSERT(foo, msg); // not supposed to compile as foo is
//                               // not a compile-time constant.
//
// - By using the type CompileAssert<(bool(expr))>, we ensures that
//   expr is a compile-time constant.  (Template arguments must be
//   determined at compile-time.)
//
// - The outter parentheses in CompileAssert<(bool(expr))> are necessary
//   to work around a bug in gcc 3.4.4 and 4.0.1.  If we had written
//
//     CompileAssert<bool(expr)>
//
//   instead, these compilers will refuse to compile
//
//     COMPILE_ASSERT(5 > 0, some_message);
//
//   (They seem to think the ">" in "5 > 0" marks the end of the
//   template argument list.)
//
// - The array size is (bool(expr) ? 1 : -1), instead of simply
//
//     ((expr) ? 1 : -1).
//
//   This is to avoid running into a bug in MS VC 7.1, which
//   causes ((0.0) ? 1 : -1) to incorrectly evaluate to 1.

#endif // _BASE_BASICTYPES_H_
