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
#include "gtest/gtest.h"
#include "base/logging.h"

#if 0
// follow test case have been success test.
TEST(logging, debug_without_initlog)
{
    DLOG(INFO) << "DLOG without_initlog";
    DLOG(WARNING) << "DLOG without_initlog";
    DLOG(ERROR) << "DLOG without_initlog";
    DLOG(ERROR_REPORT) << "DLOG without_initlog";
    DLOG(FATAL) << "DLOG without_initlog";
    DLOG(DFATAL) << "DLOG without_initlog";

    // show if conditon true
    DLOG_IF(INFO, 1 == 1) << "DLOG_IF 1 == 1";
    DLOG_IF(WARNING, 1 == 1) << "DLOG_IF 1 == 1";
    DLOG_IF(ERROR, 1 == 1) << "DLOG_IF 1 == 1";
    DLOG_IF(ERROR_REPORT, 1 == 1) << "DLOG_IF 1 == 1";
    DLOG_IF(FATAL, 1 == 1) << "DLOG_IF 1 == 1";
    DLOG_IF(DFATAL, 1 == 1) << "DLOG_IF 1 == 1";

    DLOG_ASSERT(1==0) << "DLOG_ASSERT";

    DCHECK(1==0) << "DCHECK";
}

TEST(logging, without_initlog)
{
    LOG(INFO) << "without_initlog";
    LOG(WARNING) << "without_initlog";
    LOG(ERROR) << "without_initlog";
    LOG(ERROR_REPORT) << "without_initlog";
    LOG(FATAL) << "without_initlog";
    LOG(DFATAL) << "without_initlog";

    SYSLOG(INFO) << "without_initlog";
    SYSLOG(WARNING) << "without_initlog";
    SYSLOG(ERROR) << "without_initlog";
    SYSLOG(ERROR_REPORT) << "without_initlog";
    SYSLOG(FATAL) << "without_initlog";
    SYSLOG(DFATAL) << "without_initlog";

    LOG_IF(INFO, 1 == 1) << "1 == 1";
    LOG_IF(WARNING, 1 == 1) << "1 == 1";
    LOG_IF(ERROR, 1 == 1) << "1 == 1";
    LOG_IF(ERROR_REPORT, 1 == 1) << "1 == 1";
    LOG_IF(FATAL, 1 == 1) << "1 == 1";
    LOG_IF(DFATAL, 1 == 1) << "1 == 1";

    SYSLOG_IF(INFO, 1 == 1) << "1 == 1";
    SYSLOG_IF(WARNING, 1 == 1) << "1 == 1";
    SYSLOG_IF(ERROR, 1 == 1) << "1 == 1";
    SYSLOG_IF(ERROR_REPORT, 1 == 1) << "1 == 1";
    SYSLOG_IF(FATAL, 1 == 1) << "1 == 1";
    SYSLOG_IF(DFATAL, 1 == 1) << "1 == 1";

    LOG_ASSERT(1==0) << "LOG_ASSERT";

    SYSLOG_ASSERT(1==0) << "SYSLOG_ASSERT";

    CHECK(1==0) << "CHECK";
}
#endif

TEST(logging, min_log_level)
{
    logging::SetLogMinLevel(logging::LOG_WARNING);
    LOG(INFO) << "failed if you see this log";
}

TEST(logging, filter)
{
    LOG(WARNING) << "not filter, show this message.";
    logging::SetLogFilterPrefix("XX");
    LOG(WARNING) << "with filter, don't show this message.";
    LOG(WARNING) << "XX filter, show this message.";
    LOG(ERROR) << "ignore filter, show this message.";
}

TEST(logging, items)
{
    logging::SetLogItems(false, false, false, false);
    LOG(ERROR) << "SetLogItems to false";
    logging::SetLogItems(true, true, true, true);
    LOG(ERROR) << "SetLogItems to true";
}

TEST(logging, initlog)
{
    logging::InitLogging("mydebug.log", logging::LOG_ONLY_TO_FILE,
                         logging::LOCK_LOG_FILE, logging::DELETE_OLD_LOG_FILE);

    logging::SetLogMinLevel(logging::LOG_INFO);
    logging::SetLogFilterPrefix(NULL);
    logging::SetLogItems(true, true, true, false);
    LOG(INFO) << "InitLogging";
}
