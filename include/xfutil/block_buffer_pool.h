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

#ifndef __xfutil_block_buffer_pool_h__
#define __xfutil_block_buffer_pool_h__

#include <vector>
#include <malloc.h>
#include <string.h>
#include "xfutil/strutil.h"

namespace xfutil
{

struct BlockBuffer
{
    byte_t* buf;
    uint32_t size;
    uint32_t capacity;
};

class BlockPool;
class BlockBufferPool
{
public:
	explicit BlockBufferPool(BlockPool& block_pool);
	~BlockBufferPool();
	
public:
    //重新分配一个buffer，至少一个block大小
    BlockBuffer* Alloc(uint32_t size);

	//清除所有数据
	void Free();

    //获取已分配的buffer集
    inline const std::vector<BlockBuffer>& BlockBuffers()
    {
        return m_bufs;
    }

private:	
	BlockPool& m_block_pool;
    std::vector<BlockBuffer> m_bufs;
	
private:
	BlockBufferPool(const BlockBufferPool&) = delete;
	BlockBufferPool& operator=(const BlockBufferPool&) = delete;
};

typedef std::shared_ptr<BlockBufferPool> BlockBufferPoolPtr;
#define NewBlockBufferPool 	std::make_shared<BlockBufferPool>

}

#endif

