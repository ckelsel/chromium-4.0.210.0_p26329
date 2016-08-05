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
#include "base/lock_impl.h"
#include "base/logging.h"

// NOTE: Although windows critical sections support recursive locks, we do not
// allow this, and we will commonly fire a DCHECK() if a thread attempts to
// acquire the lock a second time (while already holding it).


LockImpl::LockImpl()
{
#if !defined(NDEBUG) && defined(OS_WIN)
    recursion_used_ = false;
    owning_thread_id_ = 0;
    recursion_count_shadow_ = 0;
#endif
    ::InitializeCriticalSectionAndSpinCount(&os_lock_, 2000);
}

LockImpl::~LockImpl()
{
    ::DeleteCriticalSection(&os_lock_);
}

bool LockImpl::Try()
{
    if (::TryEnterCriticalSection(&os_lock_) != FALSE)
    {
#if !defined(NDEBUG) && defined(OS_WIN)
        // ONLY access data after locking.
        owning_thread_id_ = base::PlatformThread::CurrentId();
        DCHECK(owning_thread_id_ != 0);
        recursion_count_shadow_++;
        if (2 == recursion_count_shadow_ && !recursion_used_)
        {
            recursion_used_ = true;
            DCHECK(false);
        }
#endif
        return true;
    }

    return false;
}

void LockImpl::Lock()
{
    ::EnterCriticalSection(&os_lock_);

#if !defined(NDEBUG) && defined(OS_WIN)
        // ONLY access data after locking.
        owning_thread_id_ = base::PlatformThread::CurrentId();
        DCHECK(owning_thread_id_ != 0);
        recursion_count_shadow_++;
        if (2 == recursion_count_shadow_ && !recursion_used_)
        {
            recursion_used_ = true;
            DCHECK(false);
        }
#endif
}

void LockImpl::Unlock()
{
#if !defined(NDEBUG) && defined(OS_WIN)
    --recursion_count_shadow_;
    DCHECK(recursion_count_shadow_ >= 0);
    owning_thread_id_ = 0;
#endif

    ::LeaveCriticalSection(&os_lock_);
}

#if !defined(NDEBUG) && defined(OS_WIN)
void LockImpl::AssertAcquired() const
{
    DCHECK(recursion_count_shadow_ > 0);
    DCHECK(owning_thread_id_ == base::PlatformThread::CurrentId());
}
#endif
