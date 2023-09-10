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

#ifndef __xfutil_pack_h__
#define __xfutil_pack_h__

#include "xfutil/types.h"
#include "xfutil/buffer.h"
#include "xfutil/strutil.h"

namespace xfutil 
{

class Packer;
typedef void (*PackStructCallback)(Packer&, void* arg);

class Packer
{
public:
    Packer(BufferPoolPtr& buf_list);

public:
    void Pack(bool v)
    {
        Pack((uint8_t)v);
    }
    void Pack(int8_t v)
    {
        Pack((uint8_t)v);
    }
    void Pack(uint8_t v);

    void Pack(int16_t v);
    void Pack(uint16_t v);
    void PackFixed(uint16_t v);    

    void Pack(int32_t v);
    void Pack(uint32_t v);
    void PackFixed(uint32_t v);    

    void Pack(int64_t v);
    void Pack(uint64_t v);
    void PackFixed(uint64_t v);   

    void Pack(float v)
    {
        union 
        { 
            float f; 
            uint32_t i; 
        }uv;
        uv.f = v;
        return PackFixed(uv.i);
    }
    void Pack(double v)
    {
        union 
        { 
            double d; 
            uint64_t l; 
        }uv;
        uv.d = v;
        return PackFixed(uv.l);
    }

    void Pack(const StrView& v);

    //结构体
    void Pack(PackStructCallback cb, void* arg)
    {
        cb(*this, arg);
    }
    
public:
    BufferPoolPtr& GetBlockBufferPool()
    {
        return m_block_buffer_pool;
    }

private:
    BufferPoolPtr m_block_buffer_pool;
    Buffer* m_block_buf;          //当前正在写的buf
    byte_t* m_ptr;

private:
	Packer(const Packer&) = delete;
	Packer& operator=(const Packer&) = delete;
};


} 

#endif

