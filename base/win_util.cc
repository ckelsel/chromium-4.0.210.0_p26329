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

#include "win_util.h"

namespace win_util {
    WinVersion GetWinVersion() {
        static bool checked_version = false;
        static WinVersion win_version = WINVERSION_PRE_2000;
        if (!checked_version) {
            OSVERSIONINFOEX version_info;
            version_info.dwOSVersionInfoSize = sizeof version_info;
            GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&version_info));
            if (version_info.dwMajorVersion == 5) {
                switch (version_info.dwMinorVersion) {
                    case 0:
                        win_version = WINVERSION_2000;
                        break;
                    case 1:
                        win_version = WINVERSION_XP;
                        break;
                    case 2:
                    default:
                        win_version = WINVERSION_SERVER_2003;
                        break;
                }
            } else if (version_info.dwMajorVersion == 6) {
                if (version_info.wProductType != VER_NT_WORKSTATION) {
                    // 2008 is 6.0, and 2008 R2 is 6.1.
                    win_version = WINVERSION_2008;
                } else {
                    if (version_info.dwMinorVersion == 0) {
                        win_version = WINVERSION_VISTA;
                    } else {
                        win_version = WINVERSION_WIN7;
                    }
                }
            } else if (version_info.dwMajorVersion > 6) {
                win_version = WINVERSION_WIN7;
            }
            checked_version = true;
        }
        return win_version;
    }
}