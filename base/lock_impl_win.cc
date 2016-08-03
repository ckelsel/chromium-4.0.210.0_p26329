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

LockImpl::LockImpl()
{
#ifndef NDEBUG
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
        return true;
    }

    return false;
}

void LockImpl::Lock()
{
    ::EnterCriticalSection(&os_lock_);
}

void LockImpl::Unlock()
{
    ::LeaveCriticalSection(&os_lock_);
}

#if !defined(NDEBUG) && defined(OS_WIN)
void LockImpl::AssertAcquired() const
{
}
#endif
