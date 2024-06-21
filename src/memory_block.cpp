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

#include <deque>
#include "memory_block.h"

namespace xfutil
{

#ifdef _DEBUG
union TestBlockMemory
{
    MemoryBlockHead block;
};
static_assert(sizeof(MemoryBlockHead) % sizeof(void*) == 0, "非8字节对齐");
#endif


void MemoryBlockInit(MemoryBlockHead* block, uint32_t block_size, uint32_t element_size)
{
    block->block_size = block_size;
    block->element_size = element_size;
    block->max_element_num = (block_size - sizeof(MemoryBlockHead)) / element_size;
    block->element_used_cnt = 0;
    block->next_element_offset = block_size - block->max_element_num * element_size;
    block->next_element_ptr = nullptr;
}

byte_t* MemoryAlloc(MemoryBlockHead* block)
{
    byte_t* element = nullptr;

    //先从cache区分配，如果没有空间，再从block中分配
    if(block->next_element_ptr != nullptr)
    {
        element = block->next_element_ptr;
        block->next_element_ptr = *(byte_t**)element;
        ++block->element_used_cnt;
    }
    else if(block->next_element_offset < block->block_size)
    {
        element = (byte_t*)block +  block->next_element_offset;
        block->next_element_offset += block->element_size;
        ++block->element_used_cnt;
    }

    return element;
}

int MemoryFree(MemoryBlockHead* block, byte_t* element)
{
    if(element < (byte_t*)block || element >= (byte_t*)block + block->block_size)
    {
        return -1;
    }
    *(byte_t**)element = block->next_element_ptr;
    block->next_element_ptr = element;
    --block->element_used_cnt;
    
    if(block->element_used_cnt == 0)
    {
        return 1;
    }
    else if(block->element_used_cnt == block->max_element_num-1)
    {
        return 0;
    }
    return -1;
}


}


