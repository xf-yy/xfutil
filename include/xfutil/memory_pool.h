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

#ifndef __xfutil_memory_pool_h__
#define __xfutil_memory_pool_h__

#include <deque>
#include <set>
#include <mutex>
#include "xfutil.h"
#include "spinlock.h"

namespace xfutil
{

struct MemoryPoolNode;

//小块内存池<4KB
class MemoryPool
{
public:
	MemoryPool(BlockPool& block_pool);
	~MemoryPool();
	
public:	
	/**初始化内存池
	 * cache_num: 缓存块的数量
	 */
	bool Init(uint32_t element_size, uint32_t cache_num);

	/**申请一个buffer，如果失败，返回nullptr*/
	byte_t* Alloc();
	
	/**释放一个buffer*/
	void Free(byte_t* element);

    //已分配的块空间大小
    uint64_t AllocatedBlockSize()
    {
        return m_allocated_block_size;
    }
    //
    uint64_t AllocatedSize()
    {
        return m_allocated_size;
    }    

private:
    MemoryPoolNode* GetFreeBlockHead();
    MemoryPoolNode* FindBlockHead(byte_t* ptr);
    
    void RemoveBlockHead(MemoryPoolNode* head);
    void AddBlockHead(MemoryPoolNode* head);

private:
    BlockPool& m_block_pool;

    std::mutex m_mutex;
    uint64_t m_allocated_block_size;
    uint64_t m_allocated_size;

    uint32_t m_element_size;
    MemoryPoolNode* m_cache_block_head;
    byte_t* m_cache_block_end;

    List m_free_list;
    List m_full_list;
    std::set<byte_t*> m_blocks; //存放block end指针

private:
	MemoryPool(const MemoryPool&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
};

}

#endif

