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
#include "base/string16.h"


TEST(char16, c16memcmp)
{
#if 0
    // FIXME build error
    base::char16 c1;
    char16 *c1 = L"abcd";
    char16 *c2 = L"abcd";

    char16 a1[] = L"AAAA";

    EXPECT_EQ(0, c16memcmp(c1, c2, sizeof(c1)));
#endif
}
