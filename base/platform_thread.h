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
#ifndef BASE_PLATFORM_THREAD_H_
#define BASE_PLATFORM_THREAD_H_

#include "base/basictypes.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif



#if defined(OS_WIN)
typedef DWORD PlatformThreadId;
typedef HANDLE PlatformThreadHandle;
#elif defined(OS_POSIX)
typedef pid_t PlatformThreadId;
typedef pthread_t PlatformThreadHandle;
#endif

class PlatformThread
{

public:

    // Gets the current thread id, which may be useful for logging purposes.
    static PlatformThreadId CurrentId();

    // Yield the current thread so another thread can be scheduled.
    static void YieldCurrentThread();

    // Sleeps for the specified duration (units are milliseconds).
    static void Sleep(int duration_ms);

    // Sets the thread name visible to a debugger.  This has no effect otherwise.
    static void SetName(const char *name);

    // Implement this interface to run code on a background thread.  Your
    // ThreadMain method will be called on the newly created thread.
    class Delegate
    {
    public:
        virtual ~Delegate() { }

        virtual void ThreadMain() = 0; 
    };

    // CreateNonJoinable() does the same thing as Create() except the thread
    // cannot be Join()'d.  Therefore, it also does not output a
    // PlatformThreadHandle.
    static bool CreateNonJoinable(size_t stack_size, Delegate *delegate);

    // Creates a new thread.  The |stack_size| parameter can be 0 to indicate
    // that the default stack size should be used.  Upon success,
    // |*thread_handle| will be assigned a handle to the newly created thread,
    // and |delegate|'s ThreadMain method will be executed on the newly created
    // thread.
    // NOTE: When you are done with the thread handle, you must call Join to
    // release system resources associated with the thread.  You must ensure that
    // the Delegate object outlives the thread.
    static bool Create(size_t stack_size, Delegate *delegate,
                       PlatformThreadHandle *thread_handle);

    // Joins with a thread created via the Create function.  This function blocks
    // the caller until the designated thread exits.  This will invalidate
    // |thread_handle|.
    static void Join(PlatformThreadHandle thread_handle);

private:

    DISALLOW_COPY_AND_ASSIGN(PlatformThread);

}; // class PlatformThread



#endif // BASE_PLATFORM_THREAD_H_
