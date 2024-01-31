/*************************************************************************
Copyright (C) 2023 The xfutil Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***************************************************************************/

#ifndef __xfutil_type_h__
#define __xfutil_type_h__

#include <string.h>
#include <string>
#include <cassert>
#include <cstddef>
#include <errno.h>
#include <stdlib.h>
#include <memory>

namespace xfutil
{

//定义整型类型
typedef unsigned char byte_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned int uint32_t;
typedef signed int int32_t;

#ifdef __linux__
typedef unsigned long uint64_t;
typedef signed long int64_t;

typedef int fileid_t;
#define INVALID_FILEID ((fileid_t)-1)

#define	LastError	errno

#else

#define LastError	GetLastError()
#endif


static_assert(sizeof(void*) == 8, "invalid void*");
static_assert(sizeof(byte_t) == 1, "invalid byte_t");
static_assert(sizeof(int8_t) == 1, "invalid int8_t");
static_assert(sizeof(uint8_t) == 1, "invalid uint8_t");
static_assert(sizeof(int16_t) == 2, "invalid int16_t");
static_assert(sizeof(uint16_t) == 2, "invalid uint16_t");
static_assert(sizeof(int32_t) == 4, "invalid int32_t");
static_assert(sizeof(uint32_t) == 4, "invalid uint32_t");
static_assert(sizeof(int64_t) == 8, "invalid int64_t");
static_assert(sizeof(uint64_t) == 8, "invalid uint64_t");
static_assert(sizeof(float) == 4, "invalid float");
static_assert(sizeof(double) == 8, "invalid double");

#define ALIGN_UP(size, align) \
	((size + (align - 1)) & ~(align - 1))
		
#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(array[0]))

#define KiB(x)	(1024ULL * (x))
#define MiB(x)	(1024ULL * KiB(x))
#define GiB(x)	(1024ULL * MiB(x))

#define MAX(x, y)     (((x) > (y)) ? (x) : (y))
#define MIN(x, y)     (((x) < (y)) ? (x) : (y))

#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))

//根据成员指针获取结构体指针
#define container_of(ptr, type, member) ({ \
	(type *)( (char *)ptr - offsetof(type, member) );})


#define INVALID_FIELD_ID     -1 
#define END_FIELD_ID         0
#define MIN_FIELD_ID         1
#define MAX_FIELD_ID         32767

}

#endif

