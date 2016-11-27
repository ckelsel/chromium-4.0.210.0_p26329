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
#include "base/debug_util.h"


namespace base {

    StackTrace::StackTrace() {
    }

#if defined(OS_WIN)

    StackTrace::StackTrace(_EXCEPTION_POINTERS *exception_pointers) {
        (void) exception_pointers;
    }

#endif


    void StackTrace::PrintBackTrace() {
    }

    void StackTrace::OutputToStream(std::ostream *os) {
        (void) os;
    }


    //static
    bool DebugUtil::SpawnDebuggerOnProcess(unsigned process_id) {
        (void) process_id;
        return false;
    }

    //static
    bool DebugUtil::BeingDebugged() {
        return false;
    }

    //static
    void DebugUtil::BreakDebugger() {
    }

} // namespace base
