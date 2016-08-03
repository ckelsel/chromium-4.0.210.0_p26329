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
#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#include <string>
#include <cstring>
#include <sstream>

#include "base/basictypes.h"

//
// Optional message capabilities
// -----------------------------
// Assertion failed messages and fatal errors are displayed in a dialog box
// before the application exits. However, running this UI creates a message
// loop, which causes application messages to be processed and potentially
// dispatched to existing application windows. Since the application is in a
// bad state when this assertion dialog is displayed, these messages may not
// get processed and hang the dialog, or the application might go crazy.
//
// Therefore, it can be beneficial to display the error dialog in a separate
// process from the main application. When the logging system needs to display
// a fatal error dialog box, it will look for a program called
// "DebugMessage.exe" in the same directory as the application executable. It
// will run this application with the message as the command line, and will
// not include the name of the application as is traditional for easier
// parsing.
//
// The code for DebugMessage.exe is only one line. In WinMain, do:
//   MessageBox(NULL, GetCommandLineW(), L"Fatal Error", 0);
//
// If DebugMessage.exe is not found, the logging code will use a normal
// MessageBox, potentially causing the problems discussed above.


// Instructions
// ------------
//
// Make a bunch of macros for logging.  The way to log things is to stream
// things to LOG(<a particular severity level>).  E.g.,
//
//   LOG(INFO) << "Found " << num_cookies << " cookies";
//
// You can also do conditional logging:
//
//   LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
// The above will cause log messages to be output on the 1st, 11th, 21st, ...
// times it is executed.  Note that the special COUNTER value is used to
// identify which repetition is happening.
//
// The CHECK(condition) macro is active in both debug and release builds and
// effectively performs a LOG(FATAL) which terminates the process and
// generates a crashdump unless a debugger is attached.
//
// There are also "debug mode" logging macros like the ones above:
//
//   DLOG(INFO) << "Found cookies";
//
//   DLOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
// All "debug mode" logging is compiled away to nothing for non-debug mode
// compiles.  LOG_IF and development flags also work well together
// because the code can be compiled away sometimes.
//
// We also have
//
//   LOG_ASSERT(assertion);
//   DLOG_ASSERT(assertion);
//
// which is syntactic sugar for {,D}LOG_IF(FATAL, assert fails) << assertion;
//
// We also override the standard 'assert' to use 'DLOG_ASSERT'.
//
// The supported severity levels for macros that allow you to specify one
// are (in increasing order of severity) INFO, WARNING, ERROR, ERROR_REPORT,
// and FATAL.
//
// Very important: logging a message at the FATAL severity level causes
// the program to terminate (after the message is logged).
//
// Note the special severity of ERROR_REPORT only available/relevant in normal
// mode, which displays error dialog without terminating the program. There is
// no error dialog for severity ERROR or below in normal mode.
//
// There is also the special severity of DFATAL, which logs FATAL in
// debug mode, ERROR_REPORT in normal mode.


namespace logging 
{

// Where to record logging output? A flat file and/or system debug log via
// OutputDebugString. Defaults on Windows to LOG_ONLY_TO_FILE, and on
// POSIX to LOG_ONLY_TO_SYSTEM_DEBUG_LOG (aka stderr).
enum LoggingDestination 
{
    LOG_NONE,
    LOG_ONLY_TO_FILE,
    LOG_ONLY_TO_SYSTEM_DEBUG_LOG,
    LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG,
};

// Indicates that the log file should be locked when being written to.
// Often, there is no locking, which is fine for a single threaded program.
// If logging is being done from multiple threads or there can be more than
// one process doing the logging, the file should be locked during writes to
// make each log outut atomic. Other writers will block.
//
// All processes writing to the log file must have their locking set for it to
// work properly. Defaults to DONT_LOCK_LOG_FILE.
enum LogLockingState
{
    LOCK_LOG_FILE,
    DONT_LOCK_LOG_FILE,
};

// On startup, should we delete or append to an existing log file (if any)?
// Defaults to APPEND_TO_OLD_LOG_FILE.
enum OldFileDeletionState
{
    DELETE_OLD_LOG_FILE,
    APPEND_TO_OLD_LOG_FILE,
};

// Sets the log file name and other global logging state. Calling this function
// is recommended, and is normally done at the beginning of application init.
// If you don't call it, all the flags will be initialized to their default
// values, and there is a race condition that may leak a critical section
// object if two threads try to do the first log at the same time.
// See the definition of the enums above for descriptions and default values.
//
// The default log file is initialized to "debug.log" in the application
// directory. You probably don't want this, especially since the program
// directory may not be writable on an enduser's system.
#if defined(OS_WIN)
void InitLogging(const wchar_t *new_log_file, LoggingDestination logging_dest,
                 LogLockingState lock_log, OldFileDeletionState delete_old);
#elif defined(OS_POSIX)
void InitLogging(const char *new_log_file, LoggingDestination logging_dest,
                 LogLockingState lock_log, OldFileDeletionState delete_old);
#endif

// Sets the log level. Anything at or above this level will be written to the
// log file/displayed to the user (if applicable). Anything below this level
// will be silently ignored. The log level defaults to 0 (everything is logged)
// if this function is not called.
void SetMinLogLevel(int32 level);

// Gets the current log level
int32 GetMinLogLevel();


// Sets the log filter prefix.  Any log message below LOG_ERROR severity that
// doesn't start with this prefix with be silently ignored.  The filter defaults
// to NULL (everything is logged) if this function is not called.  Messages
// with severity of LOG_ERROR or higher will not be filtered.
void SetLogFilterPrefix(const char *filter);

// Sets the common items you want to be prepended to each log message.
// process and thread IDs default to off, the timestamp defaults to on.
// If this function is not called, logging defaults to writing the timestamp
// only.
void SetLogItems(bool enable_process_id, bool enable_thread_id,
                 bool enable_timestamp, bool enable_tickcount);

// Sets the Log Assert Handler that will be used to notify of check failures.
// The default handler shows a dialog box and then terminate the process,
// however clients can use this function to override with their own handling
// (e.g. a silent one for Unit Tests)
typedef void (*LogAssertHandlerFunction)(const std::string &str);
void SetLogAssertHandler(LogAssertHandlerFunction handler);

// Sets the Log Report Handler that will be used to notify of check failures
// // in non-debug mode. The default handler shows a dialog box and continues
// the execution, however clients can use this function to override with their
// own handling.
typedef void (*LogReportHandlerFunction)(const std::string &str);
void SetReportHandlerFunction(LogReportHandlerFunction handler);

typedef int32 LogSeverity;
const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_ERROR_REPORT = 3;
const LogSeverity LOG_FATAL = 4;
const LogSeverity LOG_NUM_SEVERITY = 5;

// LOG_DFATAL_LEVEL is LOG_FATAL in debug mode, ERROR_REPORT in normal mode
#if defined(NDEBUG)
const LogSeverity LOG_DFATAL_LEVEL = LOG_ERROR_REPORT;
#else
const LogSeverity LOG_DFATAL_LEVEL = LOG_FATAL;
#endif

// A few definitions of macros that don't generate much code. These are used
// by LOG() and LOG_IF, etc. Since these are used all over our code, it's
// better to have compact code for these operations.
#define COMPACT_GOOGLE_LOG_INFO \
    logging::LogMessage(__FILE__, __LINE__)
#define COMPACT_GOOGLE_LOG_WARNING \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_WARNING)
#define COMPACT_GOOGLE_LOG_ERROR \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_ERROR)
#define COMPACT_GOOGLE_LOG_ERROR_REPORT \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_ERROR_REPORT)
#define COMPACT_GOOGLE_LOG_FATAL \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_FATAL)
#define COMPACT_GOOGLE_LOG_DFATAL \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_DFATAL_LEVEL)

// wingdi.h defines ERROR to be 0. When we call LOG(ERROR), it gets
// substituted with 0, and it expands to COMPACT_GOOGLE_LOG_0. To allow us
// to keep using this syntax, we define this macro to do the same thing
// as COMPACT_GOOGLE_LOG_ERROR, and also define ERROR the same way that
// the Windows SDK does for consistency.
#define ERROR 0
#define COMPACT_GOOGLE_LOG_0 \
    logging::LogMessage(__FILE__, __LINE__, logging::LOG_ERROR)

// We use the preprocessor's merging operator, "##", so that, e.g.,
// LOG(INFO) becomes the token COMPACT_GOOGLE_LOG_INFO.  There's some funny
// subtle difference between ostream member streaming functions (e.g.,
// ostream::operator<<(int) and ostream non-member streaming functions
// (e.g., ::operator<<(ostream&, string&): it turns out that it's
// impossible to stream something like a string directly to an unnamed
// ostream. We employ a neat hack by calling the stream() member
// function of LogMessage which seems to avoid the problem.
#define LOG(severity) COMPACT_GOOGLE_LOG_ ## severity.stream()
#define SYSLOG(severity) LOG(severity)

#define LOG_IF(severity, condition) \
    !(condition) ? (void) 0 : logging::LogMessageVoidify() & LOG(severity) 
#define SYSLOG_IF(severity, condition) LOG_IF(severity, condition)

#define LOG_ASSERT(condition) \
    LOG_IF(FATAL, !(condition)) << "Assert failed: " #condition "."
#define SYSLOG_ASSERT(condition) \
    SYSLOG_IF(FATAL, !(condition)) << "Assert failed: " #condition "."

// CHECK dies with a fatal error if condition is not true.  It is *not*
// controlled by NDEBUG, so the check will be executed regardless of
// compilation mode.
#define CHECK(condition) \
    LOG_IF(FATAL, !(condition)) << "Check failed: " #condition "."



// Plus some debug-logging macros that get compiled to nothing for production
//
// DEBUG_MODE is for uses like
//   if (DEBUG_MODE) foo.CheckThatFoo();
// instead of
//   #ifndef NDEBUG
//     foo.CheckThatFoo();
//   #endif

#ifndef NDEBUG

#define DLOG(severity) LOG(severity)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#define DLOG_ASSERT(condition) LOG_ASSERT(condition)

// debug-only checking.  not executed in NDEBUG mode.
enum { DEBUG_MODE = 1 };
#define DCHECK(condition) CHECK(condition)

#else // NDEBUG

#define DLOG(severity) \
    true ? (void) 0 : logging::LogMessageVoidify() & LOG(severity)

#define DLOG_IF(severity, condition) \
    true ? (void) 0 : logging::LogMessageVoidify() & LOG(severity)

#define DLOG_ASSERT(condition) \
    true ? (void) 0 : LOG_ASSERT(condition) 

enum { DEBUG_MODE = 0 };
#define DCHECK(condition) \
    true ? (void) 0 : CHECK(condition)

#endif // NDEBUG

#define NOTREACHED() DCHECK(false)


// This class more or less represents a particular log message.  You
// // create an instance of LogMessage and then stream stuff to it.
// When you finish streaming to it, ~LogMessage is called and the
// full message gets streamed to the appropriate destination.
//
// You shouldn't actually use LogMessage's constructor to log things,
// though.  You should use the LOG() macro (and variants thereof)
// above.
class LogMessage
{

public:

    LogMessage(const char *file, int32 line, LogSeverity severity, int32 ctr);

    // Two special constructors that generate reduced amounts of code at
    // LOG call sites for common cases.
    //
    // Used for LOG(INFO): Implied are:
    // severity = LOG_INFO, ctr = 0
    //
    // Using this constructor instead of the more complex constructor above
    // saves a couple of bytes per call site.
    LogMessage(const char *file, int32 line);


      // Used for LOG(severity) where severity != INFO.  Implied
      // are: ctr = 0
      //
      // Using this constructor instead of the more complex constructor above
      // saves a couple of bytes per call site.
    LogMessage(const char *file, int32 line, LogSeverity severity);
    
    ~LogMessage();

    std::ostream &stream() { return stream_; }

private:
    
    void Init(const char *file, int32 line);

    LogSeverity severity_;
    std::ostringstream stream_;
    int32 message_start_;

#if defined(OS_WIN)
      // Stores the current value of GetLastError in the constructor and restores
      // it in the destructor by calling SetLastError.
      // This is useful since the LogMessage class uses a lot of Win32 calls
      // that will lose the value of GLE and the code that called the log function
      // will have lost the thread error value when the log call returns.
    class SaveLastError
    {
    public:
        SaveLastError() : error_(::GetLastError()) { }

        ~SaveLastError() { ::SetLastError(error_); }

    private:
        uint32 error_;
    };

    SaveLastError last_error_;
#endif

    DISALLOW_COPY_AND_ASSIGN(LogMessage);
}; // class LogMessage

// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class LogMessageVoidify
{
public:

    LogMessageVoidify() { }

    void operator&(std::ostream&) { }
};

} // namespace logging 


#endif // BASE_LOGGING_H_
