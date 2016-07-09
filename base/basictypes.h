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
#ifndef _BASE_BASICTYPES_H_
#define _BASE_BASICTYPES_H_

#include "base/port.h"

//
// signed
//
typedef signed char     schar;

typedef signed char     int8;

typedef signed short    int16;

typedef signed int      int32;

typedef signed long long    int64;

//
// unsigned
//
typedef unsigned char   uchar;

typedef unsigned char   uint8;

typedef unsigned short  uint16;

typedef unsigned int    uint32;

typedef unsigned long long  uint64;

//
// Range
//
const uint8  kuint8max  = (( uint8) 0xFF);
const uint16 kuint16max = ((uint16) 0xFFFF);
const uint32 kuint32max = ((uint32) 0xFFFFFFFF);
const uint64 kuint64max = ((uint64) GG_ULONGLONG(0xFFFFFFFFFFFFFFFF));

const  int8  kint8max   = ((  int8) 0x7F);
const  int8  kint8min   = ((  int8) 0x80);
const  int16 kint16max  = (( int16) 0x7FFF);
const  int16 kint16min  = (( int16) 0x8000);
const  int32 kint32max  = (( int32) 0x7FFFFFFF);
const  int32 kint32min  = (( int32) 0x80000000);
const  int64 kint64max  = (( int64) GG_LONGLONG(0x7FFFFFFFFFFFFFFF));
const  int64 kint64min  = (( int64) GG_LONGLONG(0x8000000000000000));

#endif // _BASE_BASICTYPES_H_
