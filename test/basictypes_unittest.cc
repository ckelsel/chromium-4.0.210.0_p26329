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
#include "base/basictypes.h"

TEST(basictypes_unittest, size)
{
    EXPECT_EQ(sizeof(schar), 1);
    EXPECT_EQ(sizeof(int8),  1);
    EXPECT_EQ(sizeof(int16), 2);
    EXPECT_EQ(sizeof(int32), 4);
    EXPECT_EQ(sizeof(int64), 8);

    EXPECT_EQ(sizeof(uchar), 1);
    EXPECT_EQ(sizeof(uint8), 1);
    EXPECT_EQ(sizeof(uint16), 2);
    EXPECT_EQ(sizeof(uint32), 4);
    EXPECT_EQ(sizeof(uint64), 8);
}

TEST(basictypes_unittest, compare)
{
    EXPECT_TRUE(kint8min < kint8max);
    EXPECT_TRUE(kint16min < kint16max);
    EXPECT_TRUE(kint32min < kint32max);
    EXPECT_TRUE(kint64min < kint64max);
}
