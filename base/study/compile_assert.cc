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

#include <iostream>

#define T0

#ifdef T0
// copy from basictypes.h

template <bool>
struct CompileAssert {
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]

#elif defined(T1)

#define COMPILE_ASSERT(expr, msg) \
    typedef char msg[(expr) ? 1 : -1]

#elif defined(T2)

// add template

template <bool>
struct CompileAssert {
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<bool(expr)> msg[(expr) ? 1 : -1]

#elif defined(T3)
// // - The outter parentheses in CompileAssert<(bool(expr))> are necessary
// //   to work around a bug in gcc 3.4.4 and 4.0.1.  If we had written
// //
// //     CompileAssert<bool(expr)>
// //
// //   instead, these compilers will refuse to compile
// //
// //     COMPILE_ASSERT(5 > 0, some_message);
// //
// //   (They seem to think the ">" in "5 > 0" marks the end of the
// //   template argument list.)
//
// CompileAssert<bool(expr)>
// CompileAssert<(bool(expr))> 

template <bool>
struct CompileAssert {
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[(expr) ? 1 : -1]

#elif defined(T4)
//   This is to avoid running into a bug in MS VC 7.1, which
//   causes ((0.0) ? 1 : -1) to incorrectly evaluate to 1.
//
// (expr) ? 1 : -1
// bool(expr) ? 1 : -1

template <bool>
struct CompileAssert {
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]
#endif

int main(int argc, char **argv)
{
#ifdef T0
    // sucess
    COMPILE_ASSERT(3, is_3_ok);
    COMPILE_ASSERT(5 > 0, gxx344_gxx401);

    //fail
    COMPILE_ASSERT(0.0, msvc_71);
    COMPILE_ASSERT(0 > 1, 0_bigger_than_1);
#elif defined(T1)
    // build success
    // a bug
    int foo = -1;
    COMPILE_ASSERT(foo, is_foo_ok);
#elif defined(T2)
    // build fail in gcc 3.4.4 and 4.0.1
    // a bug in gcc 3.4.4 and 4.0.1
    COMPILE_ASSERT(5 > 0, gxx344_gxx401);
#elif defined(T3)
    // build success with MS VC 7.1
    // a bug in MS VC 7.1
    COMPILE_ASSERT(0.0, msvc_71);
#elif defined(T4)
    COMPILE_ASSERT(0 > 1, 0_bigger_than_1);
#endif
    return 0;
}
