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

#include "xfutil/block_buffer_pool.h"
#include "xfutil/types.h"
#include "xfutil/block_pool.h"

namespace xfutil
{

BlockBufferPool::BlockBufferPool(BlockPool& block_pool)
	: m_block_pool(block_pool)
{
	assert(m_block_pool.BlockSize() >= 1024);
	m_bufs.reserve(32);
}

BlockBufferPool::~BlockBufferPool() 
{
	Free();
}

void BlockBufferPool::Free()
{
	for(size_t i = 0; i < m_bufs.size(); ++i) 
	{
        BlockBuffer& buf = m_bufs[i];
        if(buf.capacity == m_block_pool.BlockSize())
        {
            m_block_pool.Free(buf.buf);
        }
        else
        {
            xfree(buf.buf);
        }
	}
	m_bufs.clear();
}

BlockBuffer* BlockBufferPool::Alloc(uint32_t size) 
{
    BlockBuffer buf;
    buf.size = 0;

	if(size <= m_block_pool.BlockSize())
	{
		buf.buf = m_block_pool.Alloc();
        buf.capacity = m_block_pool.BlockSize();
    }
    else
    {
        size = ALIGN_UP(size, 4096);

		buf.buf = xmalloc(size);
		buf.capacity = size;
	}
	
    m_bufs.push_back(buf);

    return &m_bufs.back();
}

} 

