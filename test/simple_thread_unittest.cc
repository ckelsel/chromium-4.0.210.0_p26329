/* Copyright 2017 kunming.xie
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
#include "base/atomic_sequence_num.h"
#include "base/lock.h"
#include "base/simple_thread.h"
#include "base/string_util.h"
#include "base/waitable_event.h"
#include "gtest/gtest.h"


namespace {
class SetIntRunner : public base::DelegateSimpleThread::Delegate {
public:
    SetIntRunner(int *ptr, int val) : ptr_(ptr), val_(val) { }
    ~SetIntRunner() { }

    virtual void Run() {
        *ptr_ = val_;
    }
private:
    int *ptr_;
    int val_;
};

class WaitEventRunner : public base::DelegateSimpleThread::Delegate {
public:
    WaitEventRunner(base::WaitableEvent *event) : event_(event) { }
    ~WaitEventRunner() { }

    virtual void Run() {
        EXPECT_FALSE(event_->IsSignaled());
        event_->Signal();
        EXPECT_TRUE(event_->IsSignaled());
    }

private:
    base::WaitableEvent *event_;
};

} // namespace

TEST(SimpleThreadTest, CreateAndJoin) {
    int stack_int = 0;

    SetIntRunner runner(&stack_int, 2);
    EXPECT_EQ(0, stack_int);

    base::DelegateSimpleThread thread(&runner, "int_setter");
    EXPECT_FALSE(thread.HasBeenStarted());
    EXPECT_FALSE(thread.HasBeenJoined());
    EXPECT_EQ(0, stack_int);

    thread.Start();
    EXPECT_TRUE(thread.HasBeenStarted());
    EXPECT_FALSE(thread.HasBeenJoined());

    thread.Join();
    EXPECT_TRUE(thread.HasBeenStarted());
    EXPECT_TRUE(thread.HasBeenJoined());
    EXPECT_EQ(2, stack_int);
}

TEST(SimpleThreadTest, WaitForEvent)
{
    base::WaitableEvent event(true, false);

    WaitEventRunner runner(&event);
    base::DelegateSimpleThread thread(&runner, "event_waiter");

    EXPECT_FALSE(event.IsSignaled());
    EXPECT_FALSE(thread.HasBeenStarted());
    EXPECT_FALSE(thread.HasBeenJoined());

    thread.Start();
    EXPECT_TRUE(thread.HasBeenStarted());

    event.Wait();
    EXPECT_TRUE(event.IsSignaled());

    thread.Join();
    EXPECT_TRUE(thread.HasBeenJoined());
}
