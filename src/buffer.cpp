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

#include "xfutil/buffer.h"
#include "xfutil/block_pool.h"

namespace xfutil
{

WriteBuffer::WriteBuffer(BlockPool& pool)
	: m_large_pool(pool)
{
	assert(m_large_pool.BlockSize() >= 1024);
	m_blocks.reserve(32);
	m_bufs.reserve(32);
	m_usage = 0;
	
	m_ptr = nullptr;
	m_size = 0;
}

WriteBuffer::~WriteBuffer() 
{
	Clear();
}

void WriteBuffer::Clear()
{
	for(size_t i = 0; i < m_blocks.size(); i++) 
	{
		m_large_pool.Free(m_blocks[i]);
	}
	m_blocks.clear();
	
	for(size_t i = 0; i < m_bufs.size(); i++) 
	{
		xfree(m_bufs[i]);
	}
	m_bufs.clear();
	
	m_usage = 0;
	
	m_ptr = nullptr;
	m_size = 0;
}

byte_t* WriteBuffer::Write(uint32_t size) 
{
	if(size >= m_large_pool.BlockSize()/2)
	{
		byte_t* ptr = xmalloc(size);
		m_bufs.push_back(ptr);
		m_usage += size;

		return ptr;
	}
	
	if(m_size < size)
	{
		byte_t* ptr = m_large_pool.Alloc();
		m_blocks.push_back(ptr);
		m_usage += m_large_pool.BlockSize();
		
		m_ptr = (byte_t*)ptr;
		m_size = m_large_pool.BlockSize();
	}
	
	byte_t* ptr = m_ptr;
	m_ptr += size;
	m_size -= size;

	return ptr;
}

BufferPool::BufferPool(BlockPool& block_pool)
	: m_block_pool(block_pool)
{
	assert(m_block_pool.BlockSize() >= 1024);
	m_bufs.reserve(32);
}

BufferPool::~BufferPool() 
{
	Free();
}

void BufferPool::Free()
{
	for(size_t i = 0; i < m_bufs.size(); ++i) 
	{
        Buffer& buf = m_bufs[i];
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

Buffer* BufferPool::Alloc(uint32_t size) 
{
    Buffer buf;
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

