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
#include <iostream>

#include "base/string_piece.h"

namespace base
{

typedef StringPiece::size_type size_type;

std::ostream &operator<<(std::ostream &o, StringPiece &piece)
{
    o.write(piece.data(), static_cast<std::streamsize>(piece.length()));
    return o;
}

bool operator==(const StringPiece &x, const StringPiece &y)
{
    if (x.length() != y.length())
    {
        return false;
    }

    return StringPiece::wordmemcmp(x.data(), y.data(), x.length()) == 0;
}

// StringPiece

void StringPiece::copy_to_string(std::string *target)
{
    target->assign(!empty() ? data() : "", length());
}

void StringPiece::append_to_string(std::string *target)
{
    if (!empty())
    {
        target->append(data(), length());
    }
}

size_type StringPiece::copy(char *buf, size_type n, size_type pos) const
{
    size_type len = std::min(length_ - pos, n);
    memcpy(buf, ptr_ + pos, len);
    return len;
}

size_type StringPiece::find(const StringPiece &s, size_type pos) const
{
    if (pos > length_)
    {
        return npos;
    }

    const char *result = std::search(ptr_ + pos, ptr_ + length_,
                                     s.ptr_, s.ptr_ + s.length_);
    const size_type xpos = result - ptr_;
    return (xpos + s.length_ <= length_) ? xpos : npos;
}

size_type StringPiece::find(char c, size_type pos) const
{
    if (pos >= length_)
    {
        return npos;
    }

    const char *result = std::find(ptr_ + pos, ptr_ + length_, c);
    return (result != ptr_ + length_) ? (result - ptr_) : npos;
}

size_type StringPiece::rfind(const StringPiece &s, size_type pos) const
{
    if (length_ < s.length_)
    {
        return npos;
    }

    if (s.empty())
    {
        return std::min(length_, pos);
    }

    const char* last = ptr_ + std::min(length_ - s.length_, pos) + s.length_;
    const char* result = std::find_end(ptr_, last, s.ptr_, s.ptr_ + s.length_);
    return result != last ? result - ptr_ : npos;
}

size_type StringPiece::rfind(char c, size_type pos) const
{
    if (length_ == 0)
    {
        return npos;
    }

    for (size_type i = std::min(pos, length_ -1);; --i)
    {
        if (ptr_[i] == c)
        {
            return i;
        }

        if (i == 0)
        {
            break;
        }
    }

    return npos;
}

// For each character in characters_wanted, sets the index corresponding
// to the ASCII code of that character to 1 in table.  This is used by
// the find_.*_of methods below to tell whether or not a character is in
// the lookup table in constant time.
// The argument `table' must be an array that is large enough to hold all
// the possible values of an unsigned char.  Thus it should be be declared
// as follows:
//   bool table[kuint8max + 1]
static inline void build_lookup_table(const StringPiece &wanted, bool *table)
{
    const size_type length = wanted.length();
    const char* const data = wanted.data();
    for (size_type i = 0; i < length; ++i)
    {
        table[static_cast<uchar>(data[i])] = true;
    }
}

size_type StringPiece::find_first_of(const StringPiece &s, size_type pos) const
{
    if (length_ == 0 || s.length_ == 0)
    {
        return npos;
    }

    if (s.length_ == 1)
    {
        return find_first_of(s.ptr_[0], pos);
    }

    bool lookup[kuint8max + 1] = { false };
    build_lookup_table(s, lookup);
    for (size_type i = pos; i < length_; ++i)
    {
        if (lookup[static_cast<uchar>(ptr_[i])])
        {
            return i;
        }
    }

    return npos;
}

size_type StringPiece::find_first_of(char c, size_type pos) const
{
    return find(c,pos);
}

size_type StringPiece::find_first_not_of(const StringPiece &s, size_type pos) const
{
    if (length_ == 0 )
    {
        return npos;
    }

    if (s.length_ == 0 )
    {
        return 0;
    }

    if (s.length_ == 1)
    {
        return find_first_not_of(s.ptr_[0], pos);
    }

    bool lookup[kuint8max + 1] = { false };
    build_lookup_table(s, lookup);
    for (size_type i = pos; i < length_; ++i)
    {
        if (!lookup[static_cast<uchar>(ptr_[i])])
        {
            return i;
        }
    }

    return npos;
}

size_type StringPiece::find_first_not_of(char c, size_type pos) const
{
    if (length_ == 0)
    {
        return npos;
    }

    for (; pos < length_; ++pos)
    {
        if (ptr_[pos] != c)
        {
            return pos;
        }
    }

    return npos;
}

size_type StringPiece::find_last_of(const StringPiece &s, size_type pos) const
{
    if (length_ == 0 || s.length_ == 0)
    {
        return npos;
    }

    if (s.length_ == 1)
    {
        return find_last_of(s.ptr_[0], pos);
    }

    bool lookup[kuint8max + 1] = { false };
    build_lookup_table(s, lookup);
    for (size_type i = std::min(pos, length_ - 1); ; --i)
    {
        if (lookup[static_cast<uchar>(ptr_[i])])
        {
            return i;
        }

        if (i == 0)
        {
            break;
        }
    }

    return npos;
}

size_type StringPiece::find_last_of(char c, size_type pos) const
{
    return rfind(c, pos);
}

size_type StringPiece::find_last_not_of(const StringPiece &s, size_type pos) const
{
    if (length_ == 0)
    {
        return npos;
    }

    size_type i = std::min(pos, length_ - 1);
    if (s.length_ == 0)
    {
        return i;
    }

    if (s.length_ == 1)
    {
        return find_last_not_of(s.ptr_[0], pos);
    }

    bool lookup[kuint8max + 1] = { false };
    build_lookup_table(s, lookup);
    for (; ; --i)
    {
        if (!lookup[static_cast<uchar>(ptr_[i])])
        {
            return i;
        }

        if (i == 0)
        {
            break;
        }
    }

    return npos;
}

size_type StringPiece::find_last_not_of(char c, size_type pos) const
{
    if (length_ == 0)
    {
        return npos;
    }

    for (size_type i = std::min(pos, length_ - 1); ; --i)
    {
        if (ptr_[i] != c)
        {
            return i;
        }

        if (i == 0)
        {
            break;
        }
    }

    return npos;
}

StringPiece StringPiece::substr(size_type pos, size_type n) const
{
    if (pos > length_)
    {
        pos = length_;
    }

    if (n > length_ - pos)
    {
        n = length_ - pos;
    }

    return StringPiece(ptr_ + pos, n);
}

const StringPiece::size_type StringPiece::npos = size_type(-1);

}
