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
#include <errno.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <stdlib.h>

#if defined(OS_OPENBSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#include "base/sys_info.h"
#include "base/string_util.h"
#include "base/logging.h"

namespace base 
{

//static 
int32 SysInfo::NumberOfProcessors() 
{
#if defined(OS_OPENBSD)
#  error "not implement"
#else
    int32 res = sysconf(_SC_NPROCESSORS_ONLN);
    if (res == -1)
    {
        NOTREACHED();
        return 1;
    }

    return res;
#endif
}

//static 
int64 SysInfo::AmountOfPhysicalMemory()
{
#if defined(OS_OPENBSD)
#  error "not implement"
#else
    int32 pages = sysconf(_SC_PHYS_PAGES);
    int32 page_size = sysconf(_SC_PAGE_SIZE);
    if (pages == -1 || page_size == -1)
    {
        NOTREACHED();
        return 0;
    }

    return static_cast<int64>(pages) * page_size;
#endif
}

inline std::string WideToUTF8(const std::wstring &wide)
{
    return "";
}

//static 
int64 SysInfo::AmountOfFreeDiskSpace(const std::wstring &path)
{
    struct statvfs stats;
    if (statvfs(WideToUTF8(path).c_str(), &stats) != 0)
    {
        return -1;
    }

    return static_cast<int64>(stats.f_bavail) * stats.f_frsize;
}

//static 
bool SysInfo::HasEnvVar(const wchar_t *var)
{
    std::string var_utf8 = WideToUTF8(std::wstring(var));
    return getenv(var_utf8.c_str()) != NULL;
}

inline std::wstring UTF8ToWide(const char *)
{
    return L"";
}

//static 
std::wstring SysInfo::GetEnvVar(const wchar_t *var)
{
    std::string var_utf8 = WideToUTF8(std::wstring(var));
    const char *value = var_utf8.c_str();
    if (!value)
    {
        return L"";
    }
    else
    {
        return UTF8ToWide(value);
    }
}

//static 
std::string SysInfo::OperatingSystemName()
{
    utsname info;
    if (uname(&info) < 0)
    {
        NOTREACHED();
        return "unknown";
    }
    return std::string(info.sysname);
}

//static 
std::string SysInfo::OperatingSystemVersion()
{
    utsname info;
    if (uname(&info) < 0)
    {
        NOTREACHED();
        return "unknown";
    }
    return std::string(info.release);
}

//static 
void SysInfo::OperatingSystemVersionNumbers(int32 *major_version,
                                  int32 *minor_version,
                                  int32 *bugfix_version)
{
    // TODO
}

//static 
std::string SysInfo::CPUArchitecture()
{
    utsname info;
    if (uname(&info) < 0)
    {
        NOTREACHED();
        return "unknown";
    }
    return std::string(info.machine);
}

//static 
void SysInfo::GetPrimaryDisplayDimensions(int32 *width, int32 *height)
{
    // TODO
}

//static 
int32 SysInfo::DisplayCount()
{
    // TODO
    return 0;
}

//static 
size_t SysInfo::VMAllocationGranularity()
{
    return getpagesize();
}

#if defined(OS_LINUX)
//static 
size_t SysInfo::MaxSharedMemorySize()
{
    // TODO
    return 0;
}
#endif

} // namespace base 
