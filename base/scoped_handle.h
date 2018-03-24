// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SCOPED_HANDLE_H_
#define BASE_SCOPED_HANDLE_H_

#include <stdio.h>

#include "base/basictypes.h"

#if defined(OS_WIN)
#include "base/scoped_handle_win.h"
#endif

class ScopedStdioHandle {
public:
    ScopedStdioHandle() : handle_(NULL) {

    }

    explicit ScopedStdioHandle(FILE *h) : handle_(h) {

    }

    ~ScopedStdioHandle() {
        Close();
    }

    void Close() {
        if (handle_ != NULL) {
            fclose(handle_);
            handle_ = NULL;
        }
    }

    void Set(FILE *h) {
        Close();

        handle_ = h;
    }

    FILE *Get() {
        return handle_;
    }

    FILE *Take() {
        FILE *f = handle_;
        handle_ = NULL;
        return f;
    }

private:
    FILE *handle_;
    DISALLOW_COPY_AND_ASSIGN(ScopedStdioHandle);
};

#endif // BASE_SCOPED_HANDLE_H_