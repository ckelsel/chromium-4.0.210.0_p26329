// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/load_log_unittest.h"
#include "net/base/load_log_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace net {
namespace {

TEST(LoadLogUtilTest, Basic) {
  scoped_refptr<LoadLog> log(new LoadLog);

  log->Add(MakeTime(1), LoadLog::TYPE_HOST_RESOLVER_IMPL, LoadLog::PHASE_BEGIN);
  log->Add(MakeTime(5), LoadLog::TYPE_HOST_RESOLVER_IMPL_OBSERVER_ONSTART,
      LoadLog::PHASE_BEGIN);
  log->Add(MakeTime(8), LoadLog::TYPE_HOST_RESOLVER_IMPL_OBSERVER_ONSTART,
      LoadLog::PHASE_END);
  log->Add(MakeTime(12), LoadLog::TYPE_CANCELLED, LoadLog::PHASE_NONE);
  log->Add(MakeTime(131), LoadLog::TYPE_HOST_RESOLVER_IMPL, LoadLog::PHASE_END);

  EXPECT_EQ(
    "t=  1: +HOST_RESOLVER_IMPL                    [dt=130]\n"
    "t=  5:   +HOST_RESOLVER_IMPL_OBSERVER_ONSTART [dt=  3]\n"
    "t=  8:   -HOST_RESOLVER_IMPL_OBSERVER_ONSTART\n"
    "t= 12:    CANCELLED\n"
    "t=131: -HOST_RESOLVER_IMPL",
    LoadLogUtil::PrettyPrintAsEventTree(log));
}

TEST(LoadLogUtilTest, UnmatchedOpen) {
  scoped_refptr<LoadLog> log(new LoadLog);

  log->Add(MakeTime(3), LoadLog::TYPE_HOST_RESOLVER_IMPL, LoadLog::PHASE_BEGIN);
  // Note that there is no matching call to PHASE_END for all of the following.
  log->Add(MakeTime(6), LoadLog::TYPE_HOST_RESOLVER_IMPL_OBSERVER_ONSTART,
      LoadLog::PHASE_BEGIN);
  log->Add(MakeTime(7), LoadLog::TYPE_HOST_RESOLVER_IMPL_OBSERVER_ONSTART,
      LoadLog::PHASE_BEGIN);
  log->Add(MakeTime(8), LoadLog::TYPE_HOST_RESOLVER_IMPL_OBSERVER_ONSTART,
      LoadLog::PHASE_BEGIN);
  log->Add(MakeTime(10), LoadLog::TYPE_CANCELLED, LoadLog::PHASE_NONE);
  log->Add(MakeTime(16), LoadLog::TYPE_HOST_RESOLVER_IMPL, LoadLog::PHASE_END);

  EXPECT_EQ(
    "t= 3: +HOST_RESOLVER_IMPL                          [dt=13]\n"
    "t= 6:   +HOST_RESOLVER_IMPL_OBSERVER_ONSTART       [dt=10]\n"
    "t= 7:     +HOST_RESOLVER_IMPL_OBSERVER_ONSTART     [dt= 9]\n"
    "t= 8:       +HOST_RESOLVER_IMPL_OBSERVER_ONSTART   [dt= 8]\n"
    "t=10:          CANCELLED\n"
    "t=16: -HOST_RESOLVER_IMPL",
    LoadLogUtil::PrettyPrintAsEventTree(log));
}

}  // namespace
}  // namespace net
