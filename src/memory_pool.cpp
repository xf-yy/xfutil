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

#include "xfutil.h"
#include "xfutil/memory_pool.h"
#include "memory_block.h"

namespace xfutil
{

MemoryPool::MemoryPool(BlockPool& block_pool) 
    : m_block_pool(block_pool)
{		
    m_used_cnt = 0;

    m_element_size = 0;
    m_cache_num = 0;
    m_cache_block_head = nullptr;
    m_cache_block_end = nullptr;

    ListInit(&m_free_list);
    ListInit(&m_full_list);
}

MemoryPool::~MemoryPool()
{
    if(m_cache_block_head != nullptr)
    {
        xfree(m_cache_block_head);
    }
}

/**初始化内存池
 * block_size: 块大小，需对齐到4096
 * cache_num: 缓存块的数量
 */
bool MemoryPool::Init(uint32_t element_size, uint32_t cache_num)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_cache_num = cache_num;
    m_element_size = ALIGN_UP(element_size, sizeof(void*));
    uint64_t total_size = (uint64_t)cache_num * m_element_size + sizeof(MemoryBlockHead);

    m_cache_block_head = (MemoryBlockHead*)xmalloc(total_size);
    if(m_cache_block_head == nullptr)
    {
        return false;
    }
    m_cache_block_end = (byte_t*)m_cache_block_head + total_size;

    MemoryBlockInit(m_cache_block_head, total_size, m_element_size);

    return true;
}

byte_t* MemoryPool::Alloc()
{
    MemoryBlockHead* head;
    byte_t* ptr;
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        ptr = MemoryAlloc(m_cache_block_head);
        if(ptr != nullptr)
        {
            ++m_used_cnt;
            return ptr;
        }
        //从block分配，如果失败，则重新分配一个block
        head = GetFreeMemoryBlock();
        if(head != nullptr)
        {
            ptr = MemoryAlloc(head);
            if(ptr != nullptr)
            {
                ++m_used_cnt;
                return ptr;
            }
            //移动到full 链表中
            ListDelete(&head->node);
            ListAddTail(&head->node, &m_full_list);
        }
    }
    //重新分配一个block
    head = (MemoryBlockHead*)m_block_pool.Alloc();
    MemoryBlockInit(head, m_block_pool.BlockSize(), m_element_size);
    ptr = MemoryAlloc(head);

    std::lock_guard<std::mutex> lock(m_mutex);

    AddMemoryBlock(head);
    ++m_used_cnt;

    return ptr;
}

void MemoryPool::Free(byte_t* element)
{
    MemoryBlockHead* head;
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if(element < m_cache_block_end && element >= (byte_t*)m_cache_block_head)
        {
            MemoryFree(m_cache_block_head, element);
            --m_used_cnt;
            return;
        }

        //根据element找到其head
        head = FindMemoryBlock(element);
        if(head == nullptr)
        {
            return;
        }
        --m_used_cnt;
        int ret = MemoryFree(head, element);
        if(ret < 0)
        {
            return;
        }
        else if(ret == 0)
        {
            //移动到free链表中
            ListDelete(&head->node);
            ListAddTail(&head->node, &m_free_list);            
            return;
        }
        else
        {
            RemoveMemoryBlock(head);
        }
    }
    m_block_pool.Free((byte_t*)head);   
}

MemoryBlockHead* MemoryPool::GetFreeMemoryBlock()
{
    if(ListEmtpy(&m_free_list))
    {
        return nullptr;
    }
    ListNode* node = ListHead(&m_free_list);
    return container_of(node, MemoryBlockHead, node);
}

MemoryBlockHead* MemoryPool::FindMemoryBlock(byte_t* ptr)
{
    auto it = m_blocks.upper_bound(ptr);
    if(it != m_blocks.end())
    {
        return (MemoryBlockHead*)(*it - m_block_pool.BlockSize());
    }

    assert(false);
    return nullptr;
}

void MemoryPool::RemoveMemoryBlock(MemoryBlockHead* head)
{
    byte_t* end = (byte_t*)head + m_block_pool.BlockSize();

    ListDelete(&head->node); 
    m_blocks.erase(end);
} 

void MemoryPool::AddMemoryBlock(MemoryBlockHead* head)
{
    byte_t* end = (byte_t*)head + m_block_pool.BlockSize();

    m_blocks.insert(end);
    ListAddHead(&head->node, &m_free_list);
}   



}


