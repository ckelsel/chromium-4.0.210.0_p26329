// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "scoped_handle_win.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(ScopedHandle, test)
{
    ScopedHandle event(CreateEvent(NULL, 0, 0, FALSE));
    ASSERT_TRUE(event.IsValid());
    
    HANDLE h = event.Take();
    ASSERT_FALSE(event.IsValid());

    ScopedHandle event2(h);
    event2.Close();
    ASSERT_FALSE(event2.IsValid());
}