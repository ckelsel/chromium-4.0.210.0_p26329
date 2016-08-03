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

const char* const log_severity_names[LOG_NUM_SEVERITY] = 
{
    "INFO", "WARNING", "ERROR", "ERROR_REPORT", "FATAL"
};

int32 log_min_level = 0;
LogLockingState lock_log_file = LOCK_LOG_FILE;

// The lock is used if log file locking is false. It helps us avoid problems
// with multiple threads writing to the log file at the same time.  Use
// LockImpl directly instead of using Lock, because Lock makes logging calls.
static LockImpl *log_lock = NULL;

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
# define ACQUIRE_MUTEX(mutex) ::WaitforSingleObject(mutex, INFINITE)
# define RELEASE_MUTEX(mutex) ReleaseMutex(mutex)
#elif defined(OS_POSIX)
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
# define ACQUIRE_MUTEX(mutex) pthread_mutex_lock(&mutex)
# define RELEASE_MUTEX(mutex) pthread_mutex_unlock(&mutex)
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
    if (log_file)
    {
        return true;
    }

    if (!log_file_name)
    {
        // Nobody has called InitLogging to specify a debug log file, so here we
        // initialize the log file name to a default.
#if defined(OS_WIN)
        // On Windows we use the same path as the exe.
        wchar_t module_name[MAX_PATH];
        GetModuleFileName(NULL, module_name, MAX_PATH);
        log_file_name = new std::wstring(module_name);
        std::wstring::size_type last_backslash = 
            log_file_name->rfind('\\', log_file_name->length());

        if (last_backslash != std::wstring::npos)
        {
            log_file_name->erase(last_backslash + 1);
        }
        *log_file_name += L"debug.log";
#elif defined(OS_POSIX)
        // On other platforms we just use the current directory.
        log_file_name = new std::string("debug.log");
#endif
    }

    if (logging_destination == LOG_ONLY_TO_FILE ||
        logging_destination == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG)
    {
#if defined(OS_WIN)
        log_file = CreateFile(log_file_name->c_str(),
                              GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
        if (log_file == INVALID_HANDLE_VALUE)
        {
            // try the current directory
            log_file = CreateFile(L".\\debug.log",
                                  GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
            if (log_file == INVALID_HANDLE_VALUE)
            {
                log_file = NULL;
                return false;
            }
        }
#elif defined(OS_POSIX)
        log_file = fopen(log_file_name->c_str(), "a");
        if (log_file == NULL)
        {
            return false;
        }
#endif
    }

    return true;
}

void InitLogMutex()
{
#if defined(OS_WIN)
    if (!log_mutex)
    {
        // \ is not a legal character in mutex names so we replace \ with /
        std::wstring safe_name(*log_file_name);
        std::replace(safe_name.begin(), safe_name.end(), '\\', '/');

        std::wstring t(L"Global\\");
        t.append(safe_name);
        log_mutex = ::CreateMutex(NULL, FALSE, t.c_str());
    }
#elif defined(OS_POSIX)
    // statically initialized
#endif
}


void InitLogging(const PathChar *new_log_file, LoggingDestination logging_dest,
                 LogLockingState lock_log, OldFileDeletionState delete_old)
{
    if (log_file)
    {
        CloseFile(log_file);
        log_file = NULL;
    }

    lock_log_file = lock_log;
    logging_destination = logging_dest;
    
    // ignore file options if logging is disabled or only to system
    if (logging_destination == LOG_NONE ||
        logging_destination == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG)
    {
        return;
    }

    if (!log_file_name)
    {
        log_file_name = new PathString();
    }
    *log_file_name = new_log_file;

    if (delete_old == DELETE_OLD_LOG_FILE)
    {
        DeleteFilePath(*log_file_name);
    }

    // initialize lock
    if (lock_log_file == LOCK_LOG_FILE)
    {
        InitLogMutex();
    }
    else if (!log_lock)
    {
        log_lock = new LockImpl();
    }

    InitializeLogFileHandle();
}

void SetLogMinLevel(int32 level)
{
    log_min_level = level;
}

int32 GetLogMinLevel()
{
    return log_min_level;
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
    : severity_ (severity)
{
    Init(file, line);
}

LogMessage::LogMessage(const char *file, int32 line)
    : severity_ (LOG_INFO)
{
    Init(file, line);
}

LogMessage::LogMessage(const char *file, int32 line, LogSeverity severity)
    : severity_ (severity)
{
    Init(file, line);
}

LogMessage::~LogMessage()
{
    if (severity_ < log_min_level)
    {
        return;
    }

    std::string str_newline(stream_.str());
#if defined(OS_WIN)
    str_newline.append("\r\n");
#elif defined(OS_POSIX)
    str_newline.append("\n");
#endif

    if (log_filter_prefix && severity_ <= kMaxFilteredLogLevel &&
        str_newline.compare(message_start_, log_filter_prefix->size(),
                            log_filter_prefix->data()) != 0)
    {
        return;
    }

    if (logging_destination == LOG_ONLY_TO_SYSTEM_DEBUG_LOG ||
        logging_destination == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG)
    {
        // System debugger
#if defined(OS_WIN)
        OutputDebugStringA(str_newline.c_str());
#endif
        fprintf(stderr, "%s", str_newline.c_str());
    }
    else if (severity_ >= kAlwaysPrintErrorLevel)
    {
        // When we're only outputting to a log file, above a certain log level, we
        // should still output to stderr so that we can better detect and diagnose
        // problems with unit tests, especially on the buildbots.
        fprintf(stderr, "%s", str_newline.c_str());
    }


    // log to file
    if (logging_destination != LOG_NONE &&
        logging_destination != LOG_ONLY_TO_SYSTEM_DEBUG_LOG &&
        InitializeLogFileHandle())
    {
        // We can have multiple threads and/or processes, so try to prevent them
        // from clobbering each other's writes.
        if (lock_log_file == LOCK_LOG_FILE)
        {
            // Ensure that the mutex is initialized in case the client app did not
            // call InitLogging. This is not thread safe. See below.
            InitLogMutex();

            ACQUIRE_MUTEX(log_mutex);
        }
        else
        {
            if (!log_lock)
            {
                log_lock = new LockImpl();
            }
            log_lock->Lock();
        }

        // write
#if defined(OS_WIN)
        SetFilePointer(log_file, 0, 0, SEEK_END);
        uint32 num_written;
        WriteFile(log_file,
                  static_cast<const void *>(str_newline.c_str()),
                  static_cast<uint32>(str_newline.length()),
                  &num_written,
                  NULL);
#elif defined(OS_POSIX)
        fprintf(log_file, "%s", str_newline.c_str());
#endif

        // unlock
        if (lock_log_file == LOCK_LOG_FILE)
        {
            RELEASE_MUTEX(log_mutex);
        }
        else
        {
            log_lock->Unlock();
        }
    }

    // TODO
    if (severity_ == LOG_FATAL)
    {
    }

}

void LogMessage::Init(const char *file, int line)
{
#if defined(OS_WIN)
    const int32 slash = '\\';
#elif defined(OS_POSIX)
    const int32 slash = '/';
#endif
    const char *last_slash = strrchr(file, slash);
    if (last_slash)
    {
        // log only file name
        file = last_slash + 1;
    }

    stream_ << '[';

    if (log_process_id)
    {
        stream_ << CurrentProcessId() << ':';
    }

    if (log_thread_id)
    {
        stream_ << CurrentThreadID() << ':';
    }

    if (log_timestamp)
    {
        time_t t = time(NULL);
        struct tm local_time = { 0 };
#if defined(OS_WIN)
        localtime_s(&local_time, &t);
#elif defined(OS_POSIX)
        localtime_r(&t, &local_time);
#endif
        struct tm *tm_time = &local_time;
        stream_ << std::setfill('0')
                << std::setw(2) << tm_time->tm_mon + 1
                << std::setw(2) << tm_time->tm_mday
                << '/'
                << std::setw(2) << tm_time->tm_hour
                << std::setw(2) << tm_time->tm_min
                << std::setw(2) << tm_time->tm_sec
                << ':';
    }

    if (log_tickcount)
    {
        stream_ << TickCount() << ':';
    }

    stream_ << log_severity_names[severity_]
            << ':' << file << "(" << line << ")] ";
    
    message_start_ = stream_.tellp();
}



} // namespace logging

