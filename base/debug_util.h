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
#ifndef BASE_DEBUG_UTIL_H_
#define BASE_DEBUG_UTIL_H_

#include <iosfwd>
#include <vector>

#include "base/basictypes.h"

#if defined(OS_WIN)
struct _EXCEPTION_POINTERS;
#endif

namespace base
{

// A stacktrace can be helpful in debugging. For example, you can include a
// stacktrace member in a object (probably around #ifndef NDEBUG) so that you
// can later see where the given object was created from.
class StackTrace
{

public:

    // Creates a stacktrace from the current location
    StackTrace();

#if defined(OS_WIN)
    // Creates a stacktrace for an exception.
    // Note: this function will throw an import not found (StackWalk64) exception
    // on system without dbghelp 5.1.
    StackTrace(_EXCEPTION_POINTERS* exception_pointers);
#endif

    // Gets an array of instruction pointer values.
    //   count: (output) the number of elements in the returned array
    const void* const* Address(size_t *count);

    // Prints a backtrace to stderr.
    void PrintBackTrace();

    // Resolves backtrace to symbols and write to stream.
    void OutputToStream(std::ostream *os);

private:

     // From http://msdn.microsoft.com/en-us/library/bb204633.aspx,
     // the sum of FramesToSkip and FramesToCapture must be less than 63,
     // so set it to 62. Even if on POSIX it could be a larger value, it usually
     // doesn't give much more information.
    static const int MAX_TRACES = 62;

    void *trace_[MAX_TRACES];

    int count_;

    DISALLOW_COPY_AND_ASSIGN(StackTrace);
}; // class StackTrace

class DebugUtil
{

public:

    // Starts the registered system-wide JIT debugger to attach it to specified
    // process.
    static bool SpawnDebuggerOnProcess(unsigned process_id);

    // Waits wait_seconds seconds for a debugger to attach to the current process.
    // When silent is false, an exception is thrown when a debugger is detected.
    static bool WaitForDebugger(int32 wait_seconds, bool silent);

    // Are we running under a debugger?
    // On OS X, the underlying mechanism doesn't work when the sandbox is enabled.
    // To get around this, this function caches its value.
    // WARNING: Because of this, on OS X, a call MUST be made to this function
    // BEFORE the sandbox is enabled.
    static bool BeingDebugged();

    // Break into the debugger, assumes a debugger is present.
    static void BreakDebugger();

}; // class DebugUtil

} // namesapce base

#endif // BASE_DEBUG_UTIL_H_
