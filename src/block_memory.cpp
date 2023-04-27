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
#include "block_memory.h"

namespace xfutil
{

#ifdef _DEBUG
union TestBlockMemory
{
    BlockMemory mem;
};
static_assert(sizeof(BlockMemory) % sizeof(void*) == 0, "非8字节对齐");
#endif


void BlockMemoryInit(BlockMemory* mem, uint32_t block_size, uint32_t element_size)
{
    mem->block_size = block_size;
    mem->element_size = element_size;
    mem->max_element_num = (block_size - sizeof(BlockMemory)) / element_size;
    mem->allocated_element_num = 0;
    mem->next_element_offset = block_size - mem->max_element_num * element_size;
    mem->next_element_ptr = nullptr;
}

byte_t* BlockMemoryAlloc(BlockMemory* mem)
{
    byte_t* element = nullptr;

    //先从cache区分配，如果没有空间，再从block中分配
    if(mem->next_element_ptr != nullptr)
    {
        element = mem->next_element_ptr;
        mem->next_element_ptr = *(byte_t**)element;
        ++mem->allocated_element_num;
    }
    else if(mem->next_element_offset < mem->block_size)
    {
        element = (byte_t*)mem +  mem->next_element_offset;
        mem->next_element_offset += mem->element_size;
        ++mem->allocated_element_num;
    }

    return element;
}

int BlockMemoryFree(BlockMemory* mem, byte_t* element)
{
    if(element < (byte_t*)mem || element >= (byte_t*)mem + mem->block_size)
    {
        return -1;
    }
    *(byte_t**)element = mem->next_element_ptr;
    mem->next_element_ptr = element;
    --mem->allocated_element_num;
    
    if(mem->allocated_element_num == 0)
    {
        return 1;
    }
    else if(mem->allocated_element_num == mem->max_element_num-1)
    {
        return 0;
    }
    return -1;
}


}


