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
#include "xfutil/coding.h"
#include "xfutil/pack.h"

using namespace xfutil;

namespace xfutil
{

Packer::Packer(BufferPoolPtr& buf_list) : m_block_buffer_pool(buf_list)
{
    m_block_buf = m_block_buffer_pool->Alloc(1);
    m_ptr = m_block_buf->buf;
}

void Packer::Pack(uint8_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + sizeof(uint8_t) > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(sizeof(uint8_t));
        m_ptr = m_block_buf->buf;
    }

    m_ptr = EncodeFixed(m_ptr, v);
    m_block_buf->size += sizeof(uint8_t);
}

void Packer::Pack(int16_t v)
{
    uint16_t uv = EncodeZigZag(v);
    return Pack(uv);
}

void Packer::Pack(uint16_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + MAX_V16_SIZE > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(MAX_V16_SIZE);
        m_ptr = m_block_buf->buf;
    }

    byte_t* ptr = EncodeVarint(m_ptr, v);
    m_block_buf->size += ptr - m_ptr;
    m_ptr = ptr;
    
}

void Packer::PackFixed(uint16_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + sizeof(uint16_t) > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(sizeof(uint16_t));
        m_ptr = m_block_buf->buf;
    }

    m_ptr = EncodeFixed(m_ptr, v);
    m_block_buf->size += sizeof(uint16_t);
    
}

void Packer::Pack(int32_t v)
{
    uint32_t uv = EncodeZigZag(v);
    return Pack(uv);
}

void Packer::Pack(uint32_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + MAX_V32_SIZE > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(MAX_V32_SIZE);
        m_ptr = m_block_buf->buf;
    }

    byte_t* ptr = EncodeVarint(m_ptr, v);
    m_block_buf->size += ptr - m_ptr;
    m_ptr = ptr;
    
}

void Packer::PackFixed(uint32_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + sizeof(uint32_t) > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(sizeof(uint32_t));
        m_ptr = m_block_buf->buf;
    }

    m_ptr = EncodeFixed(m_ptr, v);
    m_block_buf->size += sizeof(uint32_t);
    
}

void Packer::Pack(int64_t v)
{
    uint64_t uv = EncodeZigZag(v);
    return Pack(uv);
}

void Packer::Pack(uint64_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + MAX_V64_SIZE > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(MAX_V64_SIZE);
        m_ptr = m_block_buf->buf;
    }

    byte_t* ptr = EncodeVarint(m_ptr, v);
    m_block_buf->size += ptr - m_ptr;
    m_ptr = ptr;
    
}

void Packer::PackFixed(uint64_t v)
{
    //保证空间足够大
    if(UNLIKELY(m_block_buf->size + sizeof(uint64_t) > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(sizeof(uint64_t));
        m_ptr = m_block_buf->buf;
    }

    m_ptr = EncodeFixed(m_ptr, v);
    m_block_buf->size += sizeof(uint64_t);
    
}

void Packer::Pack(const StrView& v)
{
    //保证空间足够大
    uint64_t ns = MAX_V16_SIZE + v.size;
    if(UNLIKELY(m_block_buf->size + ns > m_block_buf->capacity))
    {
        m_block_buf = m_block_buffer_pool->Alloc(ns);
        m_ptr = m_block_buf->buf;
    }

    byte_t* ptr = EncodeString(m_ptr, v);
    m_block_buf->size += ptr - m_ptr;
    m_ptr = ptr;    
}


}   


