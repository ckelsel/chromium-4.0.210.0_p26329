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

#include <wchar.h>

#include "base/sys_string_conversions.h"
#include "base/string_piece.h"
#include "base/string_util.h"

namespace base {

    std::string SysWideToUTF8(const std::wstring &wide) {
        (void) wide;
        return NULL;
    }

    std::wstring SysUTF8ToWide(const StringPiece &utf8) {
        (void) utf8;
        return NULL;
    }

}
