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
#if defined(OS_WIN)
#include <windows.h>
typedef HANDLE FileHandle;
typedef HANDLE MutexHandle;
#elif defined(OS_LINUX)
#include <sys/syscall.h>
#include <time.h>
#endif

#if defined(OS_POSIX)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAX_PATH PATH_MAX
typedef FILE* FileHandle;
typedef pthread_mutex_t* MutexHandle;
#endif

#include <ctime>
#include <iomanip>
#include <cstring>
#include <algorithm>

#include "base/logging.h"
#include "base/string_piece.h"
#include "base/string_util.h"

namespace logging
{

bool g_enable_dcheck = false;

const char* const log_severity_names[LOG_NUM_SEVERITY] = 
{
    "INFO", "WARNING", "ERROR", "ERROR_REPORT", "FATAL"
};

int32 min_log_level = 0;
LogLockingState lock_log_file = LOCK_LOG_FILE;

// The default set here for logging_destination will only be used if
// InitLogging is not called.  On Windows, use a file next to the exe;
// on POSIX platforms, where it may not even be possible to locate the
// executable on disk, use stderr.
#if defined(OS_WIN)
LoggingDestination logging_destination = LOG_ONLY_TO_FILE;
#elif defined(OS_POSIX)
LoggingDestination logging_destination = LOG_ONLY_TO_SYSTEM_DEBUG_LOG;
#endif

const int32 kMaxFilteredLogLevel = LOG_WARNING;
std::string *log_filter_prefix;

// For LOG_ERROR and above, always print to stderr.
const int32 kAlwaysPrintErrorLevel = LOG_ERROR;

// Which log file to use? This is initialized by InitLogging or
// will be lazily initialized to the default value when it is
// first needed.
#if defined(OS_WIN)
typedef wchar_t PathChar;
typedef std::wstring PathString;
#elif defined(OS_POSIX)
typedef char PathChar;
typedef std::string PathString;
#endif
PathString *log_file_name = NULL;

} // namespace logging
