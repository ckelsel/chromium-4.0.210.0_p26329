// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOCK_H_
#define BASE_LOCK_H_

#include "base/lock_impl.h"

// A convenient wrapper for an OS specific critical section.

class Lock {
public:
    Lock() {

    }

    ~Lock() {

    }

    void Acquire() {
        lock_.Lock();
    }

    void Release() {
        lock_.Unlock();
    }

    bool Try() {
        lock_.Try();
    }

    void AssertAcquired() const {
        lock_.AssertAcquired();
    }

    LockImpl *lock_impl() {
        return &lock_;
    }

private:
    LockImpl lock_;
    DISALLOW_COPY_AND_ASSIGN(Lock);
};

#endif  // BASE_LOCK_H_