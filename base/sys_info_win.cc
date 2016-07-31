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
#include <windows.h>

#include "base/sys_info.h"
#include "base/logging.h"
#include "string_util.h"

namespace base
{

// static 
int32 SysInfo::NumberOfProcessors()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return static_cast<int32>(info.dwNumberOfProcessors);
}

// static 
int64 SysInfo::AmountOfPhysicalMemory()
{
    MEMORYSTATUSEX info;
    info.dwLength = sizeof(info);
    if (!GlobalMemoryStatusEx(&info))
    {
        NOTREACHED();
        return 0;
    }

    int64 ret = static_cast<int64>(info.ullTotalPhys);
    if (ret < 0)
    {
        ret = kint64max;
    }

    return ret;
}


// static 
int64 SysInfo::AmountOfFreeDiskSpace(const std::wstring &path)
{
    ULARGE_INTEGER available, total, free;
    if (!GetDiskFreeSpaceExW(path.c_str(), &available, &total, &free))
    {
        return -1;
    }

    int64 ret = static_cast<int64>(available.QuadPart);
    if (ret < 0)
    {
        ret = kint64max;
    }

    return ret;
}

// static 
bool SysInfo::HasEnvVar(const wchar_t *var)
{
    return GetEnvironmentVariable(var, NULL, 0) != 0;
}

// static 
std::wstring SysInfo::GetEnvVar(const wchar_t *var)
{
    //TODO
    return L"";
}

// static 
std::string SysInfo::OperatingSystemName()
{
    return "Windows NT";
}

// static 
std::string SysInfo::OperatingSystemVersion()
{
    OSVERSIONINFO info = { 0 };
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionEx(&info);

    // TODO
    return L"";
}

// static 
void SysInfo::OperatingSystemVersionNumbers(int32 *major_version
                                          int32 *minor_version,
                                          int32 *bugfix_version)
{
    OSVERSIONINFO info = { 0 };
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionEx(&info);

    *major_version  = info.dwMajorVersion;
    *minor_version  = info.dwMinorVersion;
    *bugfix_version = 0;
}

// static 
std::string SysInfo::CPUArchitecture()
{
    return "x86";
}

// static 
void SysInfo::GetPrimaryDisplayDimensions(int32 *width, int32 *height)
{
    if (width)
    {
        *width = GetSystemMetrics(SM_CXSCREEN);
    }

    if (height)
    {
        *height = GetSystemMetrics(SM_CYSCREEN);
    }
}

// static 
int32 SysInfo::DisplayCount()
{
    return GetSystemMetrics(SM_CMONITORS);
}

// static 
size_t SysInfo::VMAllocationGranularity()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    return info.dwAllocationGranularity;
}

}
