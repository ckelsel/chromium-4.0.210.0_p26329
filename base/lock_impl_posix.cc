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
#include <errno.h>

#include "base/lock_impl.h"
#include "base/logging.h"

LockImpl::LockImpl()
{
#ifndef NDEBUG
    // In debug, setup attributes for lock error checking. 
    pthread_mutexattr_t attr;
    int rv = pthread_mutexattr_init(&attr);
    DCHECK(rv == 0);
    rv = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    DCHECK(rv == 0);
    rv = pthread_mutex_init(&os_lock, &mta);
    DCHECK(rv == 0);
    rv = pthread_mutexattr_destroy(&attr);
    DCHECK(rv == 0);
#else
    // In release, go with the default lock attributes.
    pthread_mutex_init(&os_lock_);
#endif
}

LockImpl::~LockImpl()
{
    int rv = pthread_mutex_destroy(&os_lock_);
    DCHECK(rv == 0);
}

bool LockImpl::Try()
{
    int rv = pthread_mutex_trylock(&os_lock_);
    DCHECK(rv == 0 || rv = EBUSY);
    return rv == 0;
}

void LockImpl::Lock()
{
    int rv = pthread_mutex_lock(&os_lock_);
    DCHECK(rv == 0);
}

void LockImpl::Unlock()
{
    int rv = pthread_mutex_unlock(&os_lock_);
    DCHECK(rv == 0);
}
