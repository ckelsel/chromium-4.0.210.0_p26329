// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "scoped_handle.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(ScopedStdioHandle, test)
{
    ScopedStdioHandle handle(fopen("c:/windows/temp/scoped_handle_unittest", "wb"));
    ASSERT_TRUE(handle.Get() != NULL);

    FILE *file = handle.Take();
    ASSERT_TRUE(handle.Get() == NULL);
    ASSERT_TRUE(file != NULL);

    ScopedStdioHandle handle2(file);
    handle2.Close();
    ASSERT_TRUE(handle2.Get() == NULL);
}