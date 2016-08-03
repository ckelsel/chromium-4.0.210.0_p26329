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
#include "build/build_config.h"

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
#include "base/lock_impl.h"

namespace logging
{

bool g_enable_dcheck = false;

const char* const log_severity_names[LOG_NUM_SEVERITY] = 
{
    "INFO", "WARNING", "ERROR", "ERROR_REPORT", "FATAL"
};

int32 min_log_level = 0;
LogLockingState lock_log_file = LOCK_LOG_FILE;

// The lock is used if log file locking is false. It helps us avoid problems
// with multiple threads writing to the log file at the same time.  Use
// LockImpl directly instead of using Lock, because Lock makes logging calls.
//static LockImpl *log_lock = NULL;

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

// When we don't use a lock, we are using a global mutex. We need to do this
// because LockFileEx is not thread safe.
#if defined(OS_WIN)
MutexHandle log_mutex = NULL;
#elif defined(OS_POSIX)
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


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

// this file is lazily opened and the handle may be NULL
FileHandle  log_file      = NULL;
PathString *log_file_name = NULL;


// what should be prepended to each message?
bool log_process_id = false;
bool log_thread_id  = false;
bool log_timestamp  = true;
bool log_tickcount  = false;

// An assert handler override specified by the client to be called instead of
// the debug message dialog and process termination.
LogAssertHandlerFunction log_assert_handler = NULL;
// An report handler override specified by the client to be called instead of
// the debug message dialog.
LogReportHandlerFunction log_report_handler = NULL;


// Helper functions to wrap platform differences

int32 CurrentProcessId()
{
#if defined(OS_WIN)
    return GetCurrentProcessId();
#elif defined(OS_POSIX)
    return getpid();
#endif
}

int32 CurrentThreadID()
{
#if defined(OS_WIN)
    return GetCurrentThreadId();
#elif defined(OS_LINUX)
    return syscall(__NR_gettid);
#endif
}

uint64 TickCount()
{
#if defined(OS_WIN)
    return GetTickCount();
#elif defined(OS_LINUX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    uint64 absolute_micro = 
        static_cast<uint64>(ts.tv_sec)  * 1000000 +
        static_cast<uint64>(ts.tv_nsec) / 1000;

    return absolute_micro;
#endif
}

void CloseFile(FileHandle log)
{
#if defined(OS_WIN)
    CloseHandle(log);
#elif defined(OS_POSIX)
    fclose(log);
#endif
}

void DeleteFilePath(const PathString &log_name)
{
#if defined(OS_WIN)
    DeleteFile(log_name.c_str());
#elif defined(OS_POSIX)
    unlink(log_name.c_str());
#endif
}

// Called by logging functions to ensure that debug_file is initialized
// and can be used for writing. Returns false if the file could not be
// initialized. debug_file will be NULL in this case.
bool InitializeLogFileHandle()
{
    return false;
}

void InitLogMutex()
{
}


void InitLogging(const PathChar *new_log_file, LoggingDestination logging_dest,
                 LogLockingState lock_log, OldFileDeletionState delete_old)
{
}

void setMinLogLevel(int32 level)
{
    min_log_level = level;
}

int32 GetMinLogLevel()
{
    return min_log_level;
}

void SetLogFilterPrefix(const char *filter)
{
    if (log_filter_prefix)
    {
        delete log_filter_prefix;
        log_filter_prefix = NULL;
    }

    if (filter)
    {
        log_filter_prefix = new std::string(filter);
    }
}

void SetLogItems(bool enable_process_id, bool enable_thread_id,
                 bool enable_timestamp, bool enable_tickcount)
{
    log_process_id = enable_process_id;
    log_thread_id  = enable_thread_id;
    log_timestamp  = enable_timestamp;
    log_tickcount  = enable_tickcount;
}

void SetLogAssertHandler(LogAssertHandlerFunction handler)
{
    log_assert_handler = handler;
}

void SetReportHandlerFunction(LogReportHandlerFunction handler)
{
    log_report_handler = handler;
}

// Displays a message box to the user with the error message in it. For
// Windows programs, it's possible that the message loop is messed up on
// a fatal error, and creating a MessageBox will cause that message loop
// to be run. Instead, we try to spawn another process that displays its
// command line. We look for "Debug Message.exe" in the same directory as
// the application. If it exists, we use it, otherwise, we use a regular
// message box.
void DisplayDebugMessage(const std::string &str)
{
}


// LogMessage

LogMessage::LogMessage(const char *file, int32 line, LogSeverity severity, int32 ctr)
{
}

LogMessage::LogMessage(const char *file, int32 line)
{
}

LogMessage::LogMessage(const char *file, int32 line, LogSeverity severity)
{
}

LogMessage::~LogMessage()
{
}

void LogMessage::Init(const char *file, int line)
{
}



} // namespace logging

