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

// A string-like object that points to a sized piece of memory.
//
// Functions or methods may use const StringPiece& parameters to accept either
// a "const char*" or a "string" value that will be implicitly converted to
// a StringPiece.  The implicit conversion means that it is often appropriate
// to include this .h file in other files rather than forward-declaring
// StringPiece as would be appropriate for most other Google classes.
//
// Systematic usage of StringPiece is encouraged as it will reduce unnecessary
// conversions from "const char*" to "string" and back again.
//
#ifndef BASE_STRING_PIECE_H_
#define BASE_STRING_PIECE_H_

#include <algorithm>
#include <iosfwd>
#include <string>

#include "base/basictypes.h"

namespace base 
{

class StringPiece
{

public:

    typedef size_t size_type;

private:

    const char *    ptr_;
    size_type       length_;

public:
    StringPiece() : ptr_(NULL), length_(0) { }
    StringPiece(const char *str) 
        : ptr_(str), length((str == NULL) ? 0 : strlen(str)) { }
    StringPiece(const std::string &str)
        : ptr_(str.data()), length_(str.length()) { }
    StringPiece(const char *offset, size_type len)
        : ptr_(offset), length_(len) { }


    // data() may not be null terminated.
    const char *data() const { return ptr_; }

    size_type size() const { return length_; }

    size_type length() const { return length_; }

    bool empty const { return length_ == 0; }


    void clear() 
    { 
        ptr_ = NULL;
        length_ = 0;
    }

    void set(const char *data, size_type len) 
    { 
        ptr_ = data; 
        length_ = len;
    }

    void set(const char *data) 
    { 
        ptr_ = data;
        length_ = (data == NULL) ? 0 : strlen(data); 
    }

    void set(const void *data, size_type len)
    {
        ptr = reinterpret_cast<const char*>(data);
        length_ = len;
    }

    char operator[](size_type i) { return ptr_[i]; }

    void remove_prefix(size_type n)
    {
        ptr_ += n;
        length_ -= n;
    }

    void remove_suffix(size_type n) { length_ -= n; }

    int compare(const StringPiece &x) const
    {
        int r = wordmemcmp(ptr_, x.ptr_, std::min(length_, x.length_));
        if (0 == r)
        {
            if (length_ < x.length_)
            {
                r = -1;
            }
            else if (length_ > x.length)
            {
                r = 1;
            }
        }

        return r;
    }

    // StringPiece(const char *offset, size_type n);
    std::string as_string() const
    {
        return std::string((length_ == 0) ? "" : ptr_, length_);
    }

    void copy_to_string(std::string *target);

    void append_to_string(std::string *target);

    bool starts_with(const StringPiece &x) const
    {
        return ((length_ >= x.length_) &&
                (wordmemcmp(ptr, x.length_, x.length_) == 0));
    }

    bool ends_with(const StringPiece &x) const
    {
        return ((length_ >= x.length_) &&
                (wordmemcmp(ptr_ + (length_ - x.length_), x.ptr_, x.length_) == 0));
    }

    // standard STL container boilerplate

    typedef char value_type;
    typedef const char *pointer;
    typedef const char &reference;
    typedef const char &const_reference;
    typedef ptrdiff_t difference_type;
    static const size_type npos;
    typedef const char *iterator;
    typedef const char *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    

    iterator begin() { return ptr_; }

    iterator end() { return ptr_ + length_; }

    const_reverse_iterator rbegin() { return ptr_ + length_; }
    
    const_reverse_iterator rend() { return ptr_; }


    size_type max_size() const { return length_; }

    size_type capacity() const { return length_; }

    size_type copy(char *buf, size_type n, size_type pos = 0) const;


    size_type find(const StringPiece &s, size_type pos = 0) const;
    size_type find(char c, size_type pos = 0) const;
    size_type rfind(const StringPiece &s, size_type pos = npos) const;
    size_type rfind(char c, size_type pos = npos) const;


    size_type find_first_of(const StringPiece &s, size_type pos = 0) const;
    size_type find_first_of(char c, size_type pos = 0) const;

    size_type find_first_not_of(const StringPiece &s, size_type pos = 0) const;
    size_type find_first_not_of(char c, size_type pos = 0) const;

    size_type find_last_of(const StringPiece &s, size_type pos = npos) const;
    size_type find_last_of(char c, size_type pos = npos) const;

    size_type find_last_not_of(const StringPiece &s, size_type pos = npos) const;
    size_type find_last_not_of(char c, size_type pos = npos) const;
    

    StringPiece substr(size_type pos, size_type n = npos) const;
    

    static int wordmemcmp(const char *p, const char *p2, size_type N)
    {
        return memcmp(p, p2, N);
    }

} // class StringPiece

bool operator==(const StringPiece &x, const StringPiece &y);

inline bool operator!=(const StringPiece &x, const StringPiece &y)
{
    return !(x == y);
}

inline bool operator<(const StringPiece &x, const StringPiece &y)
{
    return x.compare(y) < 0;
}

inline bool operator>(const StringPiece &x, const StringPiece &y)
{
    return y < x;
}

inline bool operator<=(const StringPiece &x, const StringPiece &y)
{
    return !(x > y);
}

inline bool operator>=(const StringPiece &x, const StringPiece &y)
{
    return !(x < y);
}

} // namespace base 

#endif // BASE_STRING_PIECE_H_
