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
#include "base/platform_thread.h"

namespace base {

    bool DebugUtil::WaitForDebugger(int32 wait_seconds, bool silent) {
        for (int32 i = 0; i < wait_seconds * 10; ++i) {
            if (BeingDebugged()) {
                if (!silent) {
                    BreakDebugger();
                }

                return true;
            }
            PlatformThread::Sleep(100);
        }

        return false;
    }


    const void *const *StackTrace::Address(size_t *count) {
        *count = count_;

        if (count_) {
            return trace_;
        }

        return NULL;
    }

}
