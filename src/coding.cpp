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

#include "xfutil/coding.h"

namespace xfutil
{

byte_t* EncodeVarint(byte_t* buf, uint64_t v)
{
    while(v >= 0x80)
    {
        *(buf++) = v | 0x80;
        v >>= 7;
    }
    *(buf++) = v;

    return buf;
}

byte_t* EncodeVarintR(byte_t* buf, uint32_t v)
{
    if(v < (1 << 7))
    {
        *buf++ = v;
        return buf;
    }
    else if(v < (1 << 14))
    {
        *(buf++) = v >> 7;
        *(buf++) = v | 0x80;
        return buf;
    }
    else if(v < (1 << 21))
    {
        *(buf++) = v >> 14;
        *(buf++) = (v >> 7) | 0x80;
        *(buf++) = v        | 0x80;
        return buf;
    }
    else if(v < (1 << 28))
    {
        *(buf++) = v >> 21;
        *(buf++) = (v >> 14) | 0x80;
        *(buf++) = (v >> 7)  | 0x80;
        *(buf++) = v         | 0x80;
        return buf;

    }
    //else
    //{
        *(buf++) = v >> 28;
        *(buf++) = (v >> 21) | 0x80;
        *(buf++) = (v >> 14) | 0x80;
        *(buf++) = (v >> 7)  | 0x80;
        *(buf++) = v         | 0x80;
        return buf;

    //}
}

bool DecodeVarint(const byte_t*& buf, const byte_t* end, uint64_t& v)
{
	v = 0;
	for (uint32_t shift = 0; shift < 64 && buf < end; shift += 7) 
	{
		uint64_t b = *(buf++);
		if(b >= 0x80) 
		{
			v |= ((b & 0x7f) << shift);
		} 
		else 
		{
			v |= (b << shift);
			return true;
		}
	}

	return false;
}

bool DecodeVarintR(const byte_t*& rbuf, const byte_t* rend, uint32_t& v)
{
	v = 0;
	for (uint32_t shift = 0; shift < 32 && rbuf < rend; shift += 7) 
	{
		uint32_t b = *(rbuf--);
		if(b >= 0x80) 
		{
			v |= ((b & 0x7f) << shift);
		} 
		else 
		{
			v |= (b << shift);
			return true;
		}
	}

	return false;
}

bool DecodeString(const byte_t* &buf, const byte_t* end, StrView& v)
{
    uint64_t len;
	if(!DecodeVarint(buf, end, len))
    {
        return false;
    }
    if(buf + len > end)
    {
        return false;
    }
    v.Set((char*)buf, len);
    buf += len;

    return true;
}

}


