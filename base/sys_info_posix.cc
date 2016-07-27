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
#include "base/sys_info.h"
#include "base/string_util.h"

#include <errno.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <unistd.h>

#if defined(OS_OPENBSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

namespace base 
{

//static 
int SysInfo::NumberOfProcessors() 
{
    return 0;
}

//static 
int64 SysInfo::AmountOfPhysicalMemory()
{
    return 0;
}


//static 
int64 SysInfo::AmountOfFreeDiskSpace(const std::wstring &path)
{
    return 0;
}

//static 
bool SysInfo::HasEnvVar(const wchar_t *var)
{
    return 0;
}

//static 
std::wstring SysInfo::GetEnvVar(const wchar_t *var)
{
    return 0;
}

//static 
std::string SysInfo::OperatingSystemName()
{
    return 0;
}

//static 
std::string SysInfo::OperatingSystemVersion()
{
    return 0;
}

//static 
void SysInfo::OperatingSystemVersionNumbers(int32 *major_version,
                                  int32 *minor_version,
                                  int32 *bugfix_version)
{
}

//static 
std::string SysInfo::CPUArchitecture()
{
    return 0;
}

//static 
void SysInfo::GetPrimaryDisplayDimensions(int *width, int *height)
{
}

//static 
int SysInfo::DisplayCount()
{
    return 0;
}

//static 
size_t SysInfo::VMAllocationGranularity()
{
    return 0;
}

#if defined(OS_LINUX)
//static 
size_t SysInfo::MaxSharedMemorySize()
{
    return 0;
}
#endif

} // namespace base 
