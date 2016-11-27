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
#include <sys/syscall.h>
#include <errno.h>
#include <sched.h>

#include "base/platform_thread.h"
#include "base/logging.h"


static void* ThreadFunc(void *closure)
{
    PlatformThread::Delegate *delegate = 
        static_cast<PlatformThread::Delegate*>(closure);
    delegate->ThreadMain();
    return NULL;
}

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

bool CreateThread(size_t stack_size, bool joinable,
                  PlatformThread::Delegate *delegate,
                  PlatformThreadHandle *thread_handle)
{
    bool success = false;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Pthreads are joinable by default, so only specify the detached attribute if
    // the thread should be non-joinable.
    if (!joinable)
    {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }

    if (stack_size > 0)
    {
        pthread_attr_setstacksize(&attr, stack_size);
    }

    success = !pthread_create(thread_handle, &attr, ThreadFunc, delegate);

    pthread_attr_destroy(&attr);
    return success;
}

//static 
bool PlatformThread::Create(size_t stack_size, Delegate *delegate,
                            PlatformThreadHandle *thread_handle)
{
    return CreateThread(stack_size, true, delegate, thread_handle);
}

//static 
bool PlatformThread::CreateNonJoinable(size_t stack_size, Delegate *delegate)
{
    PlatformThreadHandle unused;

    return CreateThread(stack_size, false, delegate, &unused);
}

//static 
void PlatformThread::Join(PlatformThreadHandle thread_handle)
{
    pthread_join(thread_handle, NULL);
}


