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
#ifndef BASE_SYS_INFO_H_
#define BASE_SYS_INFO_H_

#include "base/basictypes.h"

#include <string>

namespace base 
{

class SysInfo 
{
public:

    // Return the number of logical processors/cores on the current machine.
    static int32 NumberOfProcessors();

    // Return the number of bytes of physical memory on the current machine.
    static int64 AmountOfPhysicalMemory();

    // Return the number of megabytes of physical memory on the current machine.
    static int64 AmountOfPhysicalMemoryMB() 
    {
        return static_cast<int32>(AmountOfPhysicalMemory() / 1024 / 1024);
    }

    // Return the available disk space in bytes on the volume containing |
    // path|, or return -1 on failure.
    static int64 AmountOfFreeDiskSpace(const std::wstring &path);

    // Return true if the given environment variable is defined.
    static bool HasEnvVar(const wchar_t *var);

    // Return the value of the given environment varable
    // or an empty string if not defined.
    static std::wstring GetEnvVar(const wchar_t *var);

    // Return the name of the host operating system.
    static std::string OperatingSystemName();

    // Return the verson of the host operating system.
    static std::string OperatingSystemVersion();

    // Retrieves detailed numeric values for the OS version.
    static void OperatingSystemVersionNumbers(int32 *major_version,
                                              int32 *minor_version,
                                              int32 *bugfix_version);

    // Return the CPU architecture of the system.
    // exact return value may differ across platforms.
    static std::string CPUArchitecture();

    // Return the pixel dimensions of the primary display via the
    // width and height parameters.
    static void GetPrimaryDisplayDimensions(int32 *width, int32 *height);

    // Return the number of displays.
    static int32 DisplayCount();

    // Return the smallest amount of memory (in bytes) which the VM system
    // will allocate.
    static size_t VMAllocationGranularity();

#if defined(OS_LINUX)
    // Return the maximum SysV shared memory segment size.
    static size_t MaxSharedMemorySize();
#endif

#if defined(OS_CHROMEOS)
    // Return the name of the version entry we wish to look up in the Linux
    // Standard Base release information file.
    static std::string GetLinuxStandardBaseVersionKey();

    // Parse /etc/lsb-release to get version information for Google Chrome OS.
    // Declared here so it can be exposed for unit testing.
    static void ParseLsbRelease(const std::string &lsb_release,
                                int32 *major_version,
                                int32 *minor_version,
                                int32 *bugfix_version);
#endif
};

}

#endif // BASE_SYS_INFO_H_
