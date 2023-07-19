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

#ifndef __xfutil_coding_h__
#define __xfutil_coding_h__

#include <string.h>
#include "xfutil/strutil.h"

namespace xfutil
{

#define MAX_V16_SIZE	(3)
#define MAX_V32_SIZE	(5)
#define MAX_V64_SIZE	(10)

//确保空间足够大
static inline byte_t* EncodeFixed(byte_t* buf, uint8_t value)
{
	buf[0] = (byte_t)(value);
	return (buf + sizeof(uint8_t));
}

static inline bool DecodeFixed(const byte_t* &buf, const byte_t* end, uint8_t& v)
{
    if(LIKELY(buf + sizeof(uint8_t) <= end))
    {
        v = (uint8_t)buf[0]; 
        buf += sizeof(uint8_t);
        return true;
    }
    return false;
}

static inline byte_t* EncodeFixed(byte_t* buf, uint16_t value)
{
	buf[0] = (byte_t)(value);
	buf[1] = (byte_t)(value >> 8);
	return (buf + sizeof(uint16_t));
}

static inline bool DecodeFixed(const byte_t* &buf, const byte_t* end, uint16_t& v)
{
    if(LIKELY(buf + sizeof(uint16_t) <= end))
    {
        v = ((uint16_t)buf[0]) | ((uint16_t)buf[1] << 8); 
        buf += sizeof(uint16_t);
        return true;
    }
    return false;
}

static inline byte_t* EncodeFixed(byte_t* buf, uint32_t value)
{
	buf[0] = (byte_t)(value);
	buf[1] = (byte_t)(value >> 8);
	buf[2] = (byte_t)(value >> 16);
	buf[3] = (byte_t)(value >> 24);
	return (buf + sizeof(uint32_t));
}

static inline bool DecodeFixed(const byte_t* &buf, const byte_t* end, uint32_t& v)
{
    if(LIKELY(buf + sizeof(uint32_t) <= end))
    {
        v =  ((uint32_t)buf[0])
            | ((uint32_t)buf[1] << 8) 
            | ((uint32_t)buf[2] << 16) 
            | ((uint32_t)buf[3] << 24);
        buf += sizeof(uint32_t);
        return true;

    }
    return false;
}

static inline byte_t* EncodeFixed(byte_t* buf, uint64_t value)
{
	buf[0] = (byte_t)(value);
	buf[1] = (byte_t)(value >> 8);
	buf[2] = (byte_t)(value >> 16);
	buf[3] = (byte_t)(value >> 24);
	buf[4] = (byte_t)(value >> 32);
	buf[5] = (byte_t)(value >> 40);
	buf[6] = (byte_t)(value >> 48);
	buf[7] = (byte_t)(value >> 56);

	return (buf + sizeof(uint64_t));
}

static inline bool DecodeFixed(const byte_t* &buf, const byte_t* end, uint64_t& v)
{
    if(LIKELY(buf + sizeof(uint64_t) <= end))
    {
        v = ((uint64_t)buf[0])
            | ((uint64_t)buf[1] << 8) 
            | ((uint64_t)buf[2] << 16) 
            | ((uint64_t)buf[3] << 24)
            | ((uint64_t)buf[4] << 32) 
            | ((uint64_t)buf[5] << 40) 
            | ((uint64_t)buf[6] << 48)
            | ((uint64_t)buf[7] << 56);
        buf += sizeof(uint64_t);
        return true;
    }
    return false;
}

static inline byte_t* EncodeFloat(byte_t* buf, float value)
{
    union 
    { 
        float f; 
        uint32_t i; 
    }uv;
    uv.f = value;
    return EncodeFixed(buf, uv.i);
}

static inline bool DecodeFloat(const byte_t* &buf, const byte_t* end, float& v)
{
    union 
    { 
        float f; 
        uint32_t i; 
    }uv;
    bool ret = DecodeFixed(buf, end, uv.i);
    v = uv.f;
    return ret;
}

static inline byte_t* EncodeDouble(byte_t* buf, double value)
{
    union 
    { 
        double d; 
        uint64_t l; 
    }uv;
    uv.d = value;
    return EncodeFixed(buf, uv.l);
}

static inline bool DecodeDouble(const byte_t* &buf, const byte_t* end, double& v)
{
    union 
    { 
        double d; 
        uint64_t l; 
    }uv;
    bool ret = DecodeFixed(buf, end, uv.l);
    v = uv.d;

    return ret;
}

static inline uint16_t EncodeZigZag(int16_t value) 
{
    return (value << 1) ^ (value >> 15);
}
static inline int16_t DecodeZigZag(uint16_t value) 
{
    return (value >> 1) ^ -static_cast<int16_t>(value & 1);
}

static inline uint32_t EncodeZigZag(int32_t value) 
{
    return (value << 1) ^ (value >> 31);
}
static inline int32_t DecodeZigZag(uint32_t value) 
{
    return (value >> 1) ^ -static_cast<int32_t>(value & 1);
}

static inline uint64_t EncodeZigZag(int64_t value) 
{
    return (value << 1) ^ (value >> 63);
}
static inline int64_t DecodeZigZag(uint64_t value) 
{
    return (value >> 1) ^ -static_cast<int64_t>(value & 1);
}

byte_t* EncodeVarint(byte_t* buf, uint16_t v);
byte_t* EncodeVarint(byte_t* buf, uint32_t v);
byte_t* EncodeVarint(byte_t* buf, uint64_t v);
static inline byte_t* EncodeVarint(byte_t* buf, int64_t v)
{
    uint64_t uv = EncodeZigZag(v);
    return EncodeVarint(buf, uv);
}
bool DecodeVarint(const byte_t*& buf, const byte_t* end, uint64_t& v);
static inline bool DecodeVarint(const byte_t*& buf, const byte_t* end, int64_t& v)
{
    uint64_t uv64;
    bool ret = DecodeVarint(buf, end, uv64);
    v = DecodeZigZag(uv64);
    return ret;    
}


static inline byte_t* EncodeVarint(byte_t* buf, int16_t v)
{
    uint16_t uv = EncodeZigZag(v);
    return EncodeVarint(buf, uv);
}
static inline bool DecodeVarint(const byte_t*& buf, const byte_t* end, uint16_t& v)
{
    uint64_t uv64;
    bool ret = DecodeVarint(buf, end, uv64);
    v = (uint16_t)uv64;
    return ret;
}

static inline bool DecodeVarint(const byte_t*& buf, const byte_t* end, int16_t& v)
{
    uint64_t uv64;
    bool ret = DecodeVarint(buf, end, uv64);
    v = DecodeZigZag((uint16_t)uv64);
    return ret;
}

static inline byte_t* EncodeVarint(byte_t* buf, int32_t v)
{
    uint32_t uv = EncodeZigZag(v);
    return EncodeVarint(buf, uv);
}
static inline bool DecodeVarint(const byte_t*& buf, const byte_t* end, uint32_t& v)
{
    uint64_t uv64;
    bool ret = DecodeVarint(buf, end, uv64);
    v = (uint32_t)uv64;
    return ret;
}

static inline bool DecodeVarint(const byte_t*& buf, const byte_t* end, int32_t& v)
{
    uint64_t uv64;
    bool ret = DecodeVarint(buf, end, uv64);
    v = DecodeZigZag((uint32_t)uv64);
    return ret;
}


static inline byte_t* EncodeString(byte_t* buf, const char* str, uint64_t str_len)
{
	buf = EncodeVarint(buf, str_len);
	memcpy(buf, str, str_len);
	return (buf + str_len);
}

static inline byte_t* EncodeString(byte_t* buf, const char* str)
{
	uint64_t str_len = (str != nullptr ? strlen(str) : 0);
	return EncodeString(buf, str, str_len);
}

static inline byte_t* EncodeString(byte_t* buf, const StrView& str)
{
	return EncodeString(buf, str.data, str.size);
}

bool DecodeString(const byte_t* &buf, const byte_t* end, StrView& v);

}

#endif

