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


// Time represents an absolute point in time, internally represented as
// microseconds (s/1,000,000) since a platform-dependent epoch.  Each
// platform's epoch, along with other system-dependent clock interface
// routines, is defined in time_PLATFORM.cc.
//
// TimeDelta represents a duration of time, internally represented in
// microseconds.
//
// TimeTicks represents an abstract time that is always incrementing for use
// in measuring time durations. It is internally represented in microseconds.
// It can not be converted to a human-readable time, but is guaranteed not to
// decrease (if the user changes the computer clock, Time::Now() may actually
// decrease or jump).
//
// These classes are represented as only a 64-bit value, so they can be
// efficiently passed by value.

#ifndef BASE_TIME_H_
#define BASE_TIME_H_

#include <time.h>
#include "base/basictypes.h"

#if defined(OS_WIN)

#include <windows.h>

#endif

namespace base {

    class Time;

    Class TimeTicks;

    // This unit test does a lot of manual time manipulation.
    class PageLoadTrackerUnitTest;

    // TimeDelta ------------------------------------------------------------------
    class TimeDelta {
    public:

    private:
        friend class Time;

        friend class TimeTicks;

        friend TimeDelta operator*(int64 a, TimeDelta td);

        // Constructs a delta given the duration in microseconds. This is private
        // to avoid confusion by callers with an integer constructor. Use
        // FromSeconds, FromMilliseconds, etc. instead.
        explicit TimeDelta(int64 delta_us) : delta_(delta_us) {

        }

        // Delta in microseconds;
        int64 delta_;
    };

    inline TimeDelta operator*(int64 a, TimeDelta td) {

    }
}


#endif //BASE_TIME_H_
