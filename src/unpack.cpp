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

#include <atomic>
#include <mutex>
#include "xfutil/unpack.h"
#include "xfutil/types.h"
#include "xfutil/coding.h"

using namespace xfutil;

namespace xfutil
{

Unpacker::Unpacker(BlockListPtr& buf_list) : m_block_list(buf_list)
{
    m_block_list_idx = -1;
    NextBlock();
}

bool Unpacker::NextBlock()
{
    const auto bufs = m_block_list->Buffers();
    if(++m_block_list_idx >= (ssize_t)bufs.size())
    {
        m_buf_end = (byte_t*)"";
        m_ptr = m_buf_end;
        return false;
    }

    const BlockBuffer& buf = bufs[m_block_list_idx];
    m_ptr = buf.buf;
    m_buf_end = buf.buf + buf.size;
    return true;
}

bool Unpacker::Unpack(uint8_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeFixed(m_ptr, m_buf_end, v);
}

bool Unpacker::Unpack(int16_t& v)
{
    uint16_t uv;
    if(!Unpack(uv))
    {
        return false;
    }
    v = DecodeZigZag(uv);
    return true;
}
bool Unpacker::Unpack(uint16_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeVarint(m_ptr, m_buf_end, v);
}

bool Unpacker::UnpackFixed(uint16_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeFixed(m_ptr, m_buf_end, v);
}

bool Unpacker::Unpack(int32_t& v)
{
    uint32_t uv;
    if(!Unpack(uv))
    {
        return false;
    }
    v = DecodeZigZag(uv);
    return true;
}
bool Unpacker::Unpack(uint32_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeVarint(m_ptr, m_buf_end, v);
}

bool Unpacker::UnpackFixed(uint32_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeFixed(m_ptr, m_buf_end, v);
}

bool Unpacker::Unpack(int64_t& v)
{
    uint64_t uv;
    if(!Unpack(uv))
    {
        return false;
    }
    v = DecodeZigZag(uv);
    return true;
}
bool Unpacker::Unpack(uint64_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeVarint(m_ptr, m_buf_end, v);
}

bool Unpacker::UnpackFixed(uint64_t& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeFixed(m_ptr, m_buf_end, v);
}

bool Unpacker::Unpack(StrView& v)
{
    if(UNLIKELY(m_ptr >= m_buf_end))
    {
        if(!NextBlock()) 
        {
            return false;
        }
    }
    return DecodeString(m_ptr, m_buf_end, v);

}

}   


