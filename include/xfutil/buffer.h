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

#ifndef __xfutil_buffer_h__
#define __xfutil_buffer_h__

#include <vector>
#include <malloc.h>
#include <string.h>
#include "xfutil/strutil.h"

namespace xfutil
{

class BlockPool;
class WriteBuffer
{
public:
	explicit WriteBuffer(BlockPool& pool);
	~WriteBuffer();
	
public:
	/**申请空间，返回数据指针*/
	byte_t* Write(uint32_t size);
	
	/**申请空间，并写入数据，返回数据指针*/
	inline byte_t* Write(const byte_t* data, uint32_t size)
	{
		byte_t* buf = Write(size);
		memcpy(buf, data, size);
		return buf;
	}

	/**清除所有数据*/
	void Clear();
	
	//已分配空间的大小
	inline uint64_t Usage() const 
	{
		return m_usage;
	}
		
private:	
	BlockPool& m_large_pool;
	byte_t* m_ptr;				//当前待分配的指针
	uint32_t m_size;			//当前可用的空间大小
	
	uint64_t m_usage;
	std::vector<byte_t*> m_blocks;
	std::vector<byte_t*> m_bufs;

private:
	WriteBuffer(const WriteBuffer&) = delete;
	WriteBuffer& operator=(const WriteBuffer&) = delete;
};

struct Buffer
{
    byte_t* buf;
    uint32_t capacity;
    uint32_t size;
};

class BufferPool
{
public:
	explicit BufferPool(BlockPool& block_pool);
	~BufferPool();
	
public:
    //重新分配一个buffer，至少一个block大小
    Buffer* Alloc(uint32_t size);

	//清除所有数据
	void Free();

    //获取已分配的buffer集
    inline const std::vector<Buffer>& BlockBuffers()
    {
        return m_bufs;
    }

private:	
	BlockPool& m_block_pool;
    std::vector<Buffer> m_bufs;
	
private:
	BufferPool(const BufferPool&) = delete;
	BufferPool& operator=(const BufferPool&) = delete;
};

typedef std::shared_ptr<BufferPool> BufferPoolPtr;
#define NewBufferPool 	std::make_shared<BufferPool>


}

#endif

