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

// The information on how to set the thread name comes from
// a MSDN article: http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
const DWORD kVCThreadNameException = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
    DWORD dwType;  // Must be 0x1000.
    LPCSTR szName;  // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;  // Reserved for future use, must be zero.
} THREADNAME_INFO;

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
