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

#include <vector>
#include <malloc.h>
#include "block_pool.h"

namespace xfutil
{

#define BLOCK_ALGIN		4096

BlockPool::BlockPool(uint32_t block_size) : m_block_size(block_size)
{
    spinlock_init(&m_lock);
    
    m_cache_start = nullptr;
    m_cache_end = nullptr;
}

BlockPool::~BlockPool()
{
    if(m_cache_start != nullptr)
    {
        xfree(m_cache_start);
        m_cache_start = nullptr;
        m_cache_end = nullptr;
    }
    
    spinlock_destroy(&m_lock);
}

bool BlockPool::Init(uint32_t cache_num)
{
	assert(m_block_size % BLOCK_ALGIN == 0);
	if(m_block_size < BLOCK_ALGIN || m_block_size % BLOCK_ALGIN != 0)
	{
		return false;
	}
	SpinLockGuard guard(m_lock);
	if(m_cache_start != nullptr)
	{
		return false;
	}
	
	uint64_t size = (uint64_t)m_block_size * cache_num;
	byte_t* buf = xmalloc(size, BLOCK_ALGIN);
	if(buf == nullptr)
	{
		return false;
	}
	m_cache_start = buf;
	m_cache_end = buf + size;
	for(uint32_t i = 0; i < cache_num; ++i)
	{
		m_free_blocks.push_back(buf);
		buf += m_block_size;
	}
	return true;
}

byte_t* BlockPool::Alloc()
{
	{
		SpinLockGuard guard(m_lock);
		if(!m_free_blocks.empty())
		{
			byte_t* buf = m_free_blocks.front();
			m_free_blocks.pop_front();
			return buf;
		}
	}
	return xmalloc(m_block_size, BLOCK_ALGIN);
}

void BlockPool::Free(byte_t* block)
{
	{
		SpinLockGuard guard(m_lock);
		if(block >= m_cache_start && block < m_cache_end)
		{
			m_free_blocks.push_front(block);
			return;
		}
	}
	xfree(block);
}
	

}


