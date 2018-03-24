// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lock.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(Lock, test)
{
    Lock lock;
    lock.Acquire();
}