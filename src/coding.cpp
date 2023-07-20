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

byte_t* EncodeVarint(byte_t* buf, uint16_t v)
{
    if(v < (1<<7))
    {
        *(buf++) = v;
    }
    else if(v < (1<<14))
    {
        *(buf++) = v | 0x80;
        *(buf++) = v>>7;
    }
    else
    {
        *(buf++) =      v | 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) = v>>14;
    }
    return buf;
}

byte_t* EncodeVarint(byte_t* buf, uint32_t v)
{
    if(v < (1<<7))
    {
        *(buf++) = v;
    }
    else if(v < (1<<14))
    {
        *(buf++) = v | 0x80;
        *(buf++) = v>>7;
    }
    else if(v < (1<<21))
    {
        *(buf++) =      v | 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) = v>>14;
    }
    else if(v < (1<<28))
    {
        *(buf++) =      v | 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) = (v>>14)| 0x80;
        *(buf++) = v>>21;
    }
    else
    {
        *(buf++) =      v | 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) = (v>>14)| 0x80;
        *(buf++) = (v>>21)| 0x80;
        *(buf++) = v>>28;
    }
    return buf;
}

byte_t* EncodeVarint(byte_t* buf, uint64_t v)
{
    if(v < (1ULL<<35))
    {
        if(v < (1<<7))
        {
            *(buf++) = v;
        }
        else if(v < (1<<14))
        {
            *(buf++) = v | 0x80;
            *(buf++) = v>>7;
        }
        else if(v < (1<<21))
        {
            *(buf++) =      v | 0x80;
            *(buf++) = (v>>7) | 0x80;
            *(buf++) = v>>14;
        }
        else if(v < (1<<28))
        {
            *(buf++) =      v | 0x80;
            *(buf++) = (v>>7) | 0x80;
            *(buf++) = (v>>14)| 0x80;
            *(buf++) = v>>21;
        }
        else
        {
            *(buf++) =      v | 0x80;
            *(buf++) = (v>>7) | 0x80;
            *(buf++) = (v>>14)| 0x80;
            *(buf++) = (v>>21)| 0x80;
            *(buf++) = v>>28;
        }
        return buf;
    }

    *(buf++) =      v | 0x80;
    *(buf++) = (v>>7) | 0x80;
    *(buf++) = (v>>14)| 0x80;
    *(buf++) = (v>>21)| 0x80;

    if(v < (1ULL<<35))
    {
        *(buf++) = v>>28;
    }
    else if(v < (1ULL<<42))
    {
        *(buf++) = (v>>28)| 0x80;
        *(buf++) = v>>35;
    }
    else if(v < (1ULL<<49))
    {
        *(buf++) = (v>>28)| 0x80;
        *(buf++) = (v>>35)| 0x80;
        *(buf++) = v>>42;
    }
    else if(v < (1ULL<<56))
    {
        *(buf++) = (v>>28)| 0x80;
        *(buf++) = (v>>35)| 0x80;
        *(buf++) = (v>>42)| 0x80;
        *(buf++) = v>>49;
    }
    else if(v < (1ULL<<63))
    {
        *(buf++) = (v>>28)| 0x80;
        *(buf++) = (v>>35)| 0x80;
        *(buf++) = (v>>42)| 0x80;
        *(buf++) = (v>>49)| 0x80;
        *(buf++) = v>>56;
    }
    else
    {
        *(buf++) = (v>>28)| 0x80;
        *(buf++) = (v>>35)| 0x80;
        *(buf++) = (v>>42)| 0x80;
        *(buf++) = (v>>49)| 0x80;
        *(buf++) = (v>>56)| 0x80;
        *(buf++) = v>>63;
    }
    return buf;
}

bool DecodeVarint(const byte_t*& buf, const byte_t* end, uint64_t& v)
{
	v = 0;
	for (uint32_t shift = 0; shift <= 63 && buf < end; shift += 7) 
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

byte_t* EncodeVarintR(byte_t* buf, uint32_t v)
{
    if(v < (1<<7))
    {
        *(buf++) = v;
    }
    else if(v < (1<<14))
    {
        *(buf++) = v>>7;
        *(buf++) = v | 0x80;
    }
    else if(v < (1<<21))
    {
        *(buf++) = v>>14;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) =      v | 0x80;
    }
    else if(v < (1<<28))
    {
        *(buf++) = v>>21;
        *(buf++) = (v>>14)| 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) =      v | 0x80;
    }
    else
    {
        *(buf++) = v>>28;
        *(buf++) = (v>>21)| 0x80;
        *(buf++) = (v>>14)| 0x80;
        *(buf++) = (v>>7) | 0x80;
        *(buf++) =      v | 0x80;
    }
    return buf;
}

bool DecodeVarintR(const byte_t*& buf, const byte_t* begin, uint32_t& v)
{
	v = 0;
	for (uint32_t shift = 0; shift < 32 && buf >= begin; shift += 7) 
	{
		uint32_t b = *(buf--);
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


