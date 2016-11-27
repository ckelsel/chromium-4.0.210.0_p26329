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
#include "win_util.h"


// The information on how to set the thread name comes from
// a MSDN article: http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
const DWORD kVCThreadNameException = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
    DWORD dwType;  // Must be 0x1000.
    LPCSTR szName;  // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;  // Reserved for future use, must be zero.
} THREADNAME_INFO;

DWORD __stdcall ThreadFunc(void *closure)
{
    PlatformThread::Delegate *delegate =
            static_cast<PlatformThread::Delegate*>(closure);
    delegate->ThreadMain();
    return NULL;
}

//static 
PlatformThreadId PlatformThread::CurrentId()
{
    return GetCurrentThreadId();
}

//static 
void PlatformThread::YieldCurrentThread()
{
    ::Sleep(0);
}

//static 
void PlatformThread::Sleep(int duration_ms)
{
    ::Sleep(duration_ms);
}

//static 
void PlatformThread::SetName(const char *name)
{
    // The debugger needs to be around to catch the name in the exception.  If
    // there isn't a debugger, we are just needlessly throwing an exception.
    if (!::IsDebuggerPresent())
    {
        return;
    }

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = CurrentId();
    info.dwFlags = 0;

    __try 
    {
        RaiseException(kVCThreadNameException, 0, sizeof(info)/sizeof(DWORD),
                       reinterpret_cast<DWORD_PTR*>(&info));
    } __except(EXCEPTION_CONTINUE_EXECUTION) 
    {
    }
}

//static
bool PlatformThread::CreateNonJoinable(size_t stack_size, Delegate *delegate)
{
    PlatformThreadHandle thread_handle;

    bool result =  Create(stack_size, delegate, &thread_handle);
    CloseHandle(thread_handle);
    return result;
}

//static
bool PlatformThread::Create(size_t stack_size, Delegate *delegate,
                   PlatformThreadHandle *thread_handle)
{
    DWORD flags = 0;
    if (stack_size > 0 && win_util::GetWinVersion() >= win_util::WINVERSION_XP) {
        flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
    } else {
        stack_size = 0;
    }

    // Using CreateThread here vs _beginthreadex makes thread creation a bit
    // faster and doesn't require the loader lock to be available.  Our code will
    // have to work running on CreateThread() threads anyway, since we run code
    // on the Windows thread pool, etc.  For some background on the difference:
    //   http://www.microsoft.com/msj/1099/win32/win321099.aspx
    *thread_handle = CreateThread(NULL, stack_size, ThreadFunc, delegate, flags, NULL);
    return *thread_handle != NULL;
}

//static
void PlatformThread::Join(PlatformThreadHandle thread_handle)
{
    // Wait for the thread to exit.  It should already have terminated but make
    // sure this assumption is valid.
    DWORD result = WaitForSingleObject(thread_handle, INFINITE);
    DCHECK_EQ(WAIT_OBJECT_0, result);

    CloseHandle(thread_handle);
}


