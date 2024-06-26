/*************************************************************************
Copyright (C) 2022 The xfutil Authors. All rights reserved.

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

#ifndef __xfutil_memory_block_h__
#define __xfutil_memory_block_h__

#include <deque>
#include "xfutil.h"
#include "xfutil/list.h"

namespace xfutil
{

struct MemoryBlockHead
{
    ListNode node;

    uint32_t block_size;
    uint32_t element_size;
    uint16_t max_element_num;
    uint16_t element_used_cnt;
    uint32_t next_element_offset;
    byte_t* next_element_ptr;
};

//初始化
void MemoryBlockInit(MemoryBlockHead* block, uint32_t block_size, uint32_t element_size);

//失败时返回nullptr
byte_t* MemoryAlloc(MemoryBlockHead* block);

//0: 可用数量从0->1，1：可用数量变满，其他返回-1
int MemoryFree(MemoryBlockHead* block, byte_t* element);

//返回已使用的元素数量
inline uint32_t MemoryUsedSize(MemoryBlockHead* block)
{
    return block->element_used_cnt * block->element_size;
}

}

#endif

