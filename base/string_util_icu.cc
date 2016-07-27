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


// UTF-8 <-> WIde
std::string WideToUTF8(const std::wstring &wide)
{
    std::string ret;
    if (wide.empty())
    {
        return ret;
    }

    WideToUTF8(wide, wide.length(), &ret);
    return ret;
}

bool WideToUTF8(const wchar_t *src, size_t src_len, std::string *output)
{
    if (src_len == 0)
    {
        output->clear();
        return true;
    }

    ReserveUTF8Output(src, src_len, output);
    return ConvertUnicode<wchar_t, std::string>(src, src_len, output);
}
