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
#ifndef BASE_LOCK_IMPL_H_
#define BASE_LOCK_IMPL_H_

#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#endif

#include "base/basictypes.h"

// This class implements the underlying platform-specific spin-lock mechanism
// used for the Lock class.  Most users should not use LockImpl directly, but
// should instead use Lock.
class LockImpl
{

public:

#if defined(OS_WIN)
    typedef CRITICAL_SECTION OSLockType;
#elif defined(OS_POSIX)
    typedef pthread_mutex_t OSLockType;
#endif

    LockImpl();

    ~LockImpl();

    // If the lock is not held, take it and return true.  If the lock is already
    // held by something else, immediately return false
    bool Try();

    // Take the lock, blocking until it is available if necessary.
    void Lock();

    // Release the lock.  This must only be called by the lock's holder: after
    // a successful call to Try, or a call to Lock.
    void Unlock();


    // Debug-only method that will DCHECK() if the lock is not acquired by the
    // current thread.  In non-debug builds, no check is performed.
    // Because linux and mac condition variables modify the underlyning lock
    // through the os_lock() method, runtime assertions can not be done on those
    // builds.
#if defined(NDEBUG) || !defined(OS_WIN)
    void AssertAcquired() const { }
#else
    void AssertAcquired() const;
#endif

private:

    OSLockType os_lock_;

    DISALLOW_COPY_AND_ASSIGN(LockImpl);
}; // class LockImpl

#endif // BASE_LOCK_IMPL_H_
