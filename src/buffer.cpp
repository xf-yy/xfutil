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

WriteBuffer::WriteBuffer(BlockPool* pool) : m_block_pool(pool), m_block_size(pool->BlockSize())
{
	m_blocks.reserve(16);
	m_bufs.reserve(16);
}

WriteBuffer::WriteBuffer(uint32_t block_size) : m_block_pool(nullptr), m_block_size(block_size)
{
	m_blocks.reserve(16);
	m_bufs.reserve(16);
}


WriteBuffer::~WriteBuffer() 
{
	Clear();
}

void WriteBuffer::Clear()
{
	m_ptr = nullptr;
	m_size = 0;

	for(size_t i = 0; i < m_blocks.size(); i++) 
	{
		m_block_pool->Free(m_blocks[i]);
	}
	m_blocks.clear();
	
	for(size_t i = 0; i < m_bufs.size(); i++) 
	{
		xfree(m_bufs[i]);
	}
	m_bufs.clear();
	
	m_usage = 0;
	
}

byte_t* WriteBuffer::Write(uint32_t size) 
{
	if(m_size >= size)
	{
		byte_t* ptr = m_ptr;
		m_ptr += size;
		m_size -= size;

		return ptr;
	}
	return LargeWrite(size);
}

byte_t* WriteBuffer::LargeWrite(uint32_t size) 
{
	assert(size > m_size);

	if(size > m_block_size/2)
	{
		byte_t* ptr = xmalloc(size);
		m_bufs.push_back(ptr);
		m_usage += size;

		return ptr;
	}
	
	if(m_block_pool != nullptr)
	{
		m_ptr = m_block_pool->Alloc();
		m_blocks.push_back(m_ptr);
	}
	else
	{
		m_ptr = xmalloc(size);
		m_bufs.push_back(m_ptr);
	}
	m_usage += m_block_size;
	m_size = m_block_size;
	
	byte_t* ptr = m_ptr;
	m_ptr += size;
	m_size -= size;

	return ptr;
}


BlockBuffer::BlockBuffer(BlockPool& block_pool) : m_block_pool(block_pool)
{
	assert(m_block_pool.BlockSize() >= 1024);
	m_blocks.reserve(16);
}

BlockBuffer::~BlockBuffer() 
{
	Free();
}

void BlockBuffer::Free()
{
	for(size_t i = 0; i < m_blocks.size(); ++i) 
	{
        Block& block = m_blocks[i];
        if(block.capacity == m_block_pool.BlockSize())
        {
            m_block_pool.Free(block.buf);
        }
        else
        {
            xfree(block.buf);
        }
	}
	m_blocks.clear();
}

Block* BlockBuffer::Alloc(uint32_t size) 
{
    Block block;
    block.size = 0;

	if(size <= m_block_pool.BlockSize())
	{
		block.buf = m_block_pool.Alloc();
        block.capacity = m_block_pool.BlockSize();
    }
    else
    {
        size = ALIGN_UP(size, 4096);

		block.buf = xmalloc(size);
		block.capacity = size;
	}
	
    m_blocks.push_back(block);

    return &m_blocks.back();
}

} 

