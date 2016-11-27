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
#ifndef BASE_NON_THREAD_SAFE_H_
#define BASE_NON_THREAD_SAFE_H_

#include "base/platform_thread.h"

// A helper class used to help verfy that methods of a class are
// called from the same thread. One can inherit from this calss and use
// CalledOnValidThread() to verify.
//
// This is intended to be used with classes that appear to be thread safe, but
// aren't. For example, a service or a singleton link the preferences system.
//
// Example:
// class MyClass : public NonThreadSafe {
//  public:
//   void foo() {
//    DCHECK(CalledOnValidThread());
//    ... (do stuff) ...
//   }
// };
//
// In Release mode, CalledOnValidThread will always return true.
//


#if defined(NDEBUG)
// Do nothing in release mode.
class NonThreadSafe {
public:
    NonThreadSafe() { }
    ~NonThreadSafe() { }

    bool CalledOnValidThread() const 
    {
        return true;
    }
};
#else
class NonThreadSafe {
public:
    NonThreadSafe();
    ~NonThreadSafe();

    bool CalledOnValidThread() const;

private:
    PlatformThreadId valid_thread_id_;
};
#endif



#endif // BASE_NON_THREAD_SAFE_H_
