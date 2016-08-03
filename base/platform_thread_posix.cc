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
#include "base/platform_thread.h"
#include "base/logging.h"

namespace base
{

//static 
PlatformThreadId PlatformThread::CurrentId()
{
    return syscall(__NR_gettid);
}

//static 
void PlatformThread::YieldCurrentThread()
{
    sched_yield();
}

//static 
void PlatformThread::Sleep(int duration_ms)
{
    struct timespec sleep_time, remaining;

    // Contains the portion of duration_ms >= 1 sec.
    sleep_time.tv_sec = duration_ms / 1000;
    duration_ms -= sleep_time.tv_sec * 1000;

    // Contains the portion of duration_ms < 1 sec.
    sleep_time.tv_nsec = duration_ms * 1000 * 1000;  // nanoseconds.

    while (nanosleep(&sleep_time, &remaining) == -1 && errno == EINTR)
    {
        sleep_time = remaining;
    }
}

//static 
void PlatformThread::SetName(const char *name)
{
    // The POSIX standard does not provide for naming threads, and neither Linux
    // nor Mac OS X (our two POSIX targets) provide any non-portable way of doing
    // it either. (Some BSDs provide pthread_set_name_np but that isn't much of a
    // consolation prize.)
}



}
