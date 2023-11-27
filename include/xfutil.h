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

#ifndef __xfutil_xfutil_h__
#define __xfutil_xfutil_h__

#include "xfutil/types.h"
#include "xfutil/list.h"
#include "xfutil/block_pool.h"
#include "xfutil/memory_pool.h"
#include "xfutil/bloom_filter.h"
#include "xfutil/buffer.h"
#include "xfutil/coding.h"
#include "xfutil/directory.h"
#include "xfutil/file.h"
#include "xfutil/file_notify.h"
#include "xfutil/hash.h"
#include "xfutil/logger.h"
#include "xfutil/lru_cache.h"
#include "xfutil/path.h"
#include "xfutil/process.h"
#include "xfutil/queue.h"
#include "xfutil/rwlock.h"
#include "xfutil/spinlock.h"
#include "xfutil/strutil.h"
#include "xfutil/sysinfo.h"
#include "xfutil/thread.h"

#if __cplusplus < 201103L
#error "only support c++ 11 or later, use -std=c++11 option for compile"
#endif

namespace xfutil 
{

//获取版本号：字符串格式
const char* GetVersionString();

//获取版本号：整型格式
uint32_t GetVersion();

} 

#endif

