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
#include "base/non_thread_safe.h"
#include "base/logging.h"

class MyClass : public NonThreadSafe
              , public PlatformThread::Delegate
{
    
    void ThreadMain()
    {
        ASSERT_FALSE(CalledOnValidThread());
    }
};

TEST(non_thread_safe, same_thread)
{
    MyClass *test = new MyClass();

    ASSERT_TRUE(test->CalledOnValidThread());

    delete test;
}

TEST(non_thread_safe, different_thread)
{
    PlatformThreadHandle thread_handle;
    MyClass *test = new MyClass();

    ASSERT_TRUE(test->CalledOnValidThread());

    bool success = PlatformThread::Create(0, test, &thread_handle);
    ASSERT_TRUE(success);
    PlatformThread::Join(thread_handle);

    delete test;
}

