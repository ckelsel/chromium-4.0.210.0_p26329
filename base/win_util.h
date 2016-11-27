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

#ifndef BASE_WIN_UTIL_H_
#define BASE_WIN_UTIL_H_

#include <windows.h>
#include <aclapi.h>

#include <string>

namespace win_util {

enum WinVersion {
    WINVERSION_PRE_2000 = 0,
    WINVERSION_2000 = 1,
    WINVERSION_XP = 2,
    WINVERSION_SERVER_2003 = 3,
    WINVERSION_VISTA = 4,
    WINVERSION_2008 = 5,
    WINVERSION_WIN7 = 6,
};

void GetNonClientMetrics(NONCLIENTMETRICS* metrics);

// Returns the running version of Windows.
WinVersion GetWinVersion();

}

#endif //BASE_WIN_UTIL_H_
