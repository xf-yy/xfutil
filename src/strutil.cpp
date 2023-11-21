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

#include "xfutil/strutil.h"

namespace xfutil
{

bool String::Reserve(size_t size)
{
	if(size <= m_capacity)
	{
		return true;
	}
	char* ptr = (char*)xmalloc(size);
	if(ptr == nullptr)
	{
		return false;
	}
	if(m_buf != nullptr)
	{
        memcpy(ptr, m_buf, m_size);
		xfree(m_buf);
	}
	m_buf = (char*)ptr;
	m_capacity = size;
	return true;
}

bool String::Resize(size_t size)
{
	if(Reserve(size))
	{
        m_size = size;
        return true;
	}
    return false;
}	

bool String::Append(const char* data, size_t size)
{
	if(m_size + size > m_capacity)
	{
		if(!Reserve(size + m_capacity*2))
		{
			return false;
		}
	}

	memcpy(m_buf+m_size, data, size);
	m_size += size;

	return true;
}


int StrView::Compare(const StrView& dst) const
{
	const size_t min_size = MIN(size, dst.size);
	int ret = memcmp(data, dst.data, min_size);
	if(ret == 0)
	{
		if(size < dst.size)
		{
			ret = -1;
		}
		else if(size > dst.size)
		{
			ret = 1;
		}
	}
	return ret;
}

uint32_t StrView::GetPrefixLength(const StrView& str) const
{
	uint32_t min_len = MIN(size, str.size);
	for(uint32_t i = 0; i < min_len; ++i)
	{
		if(data[i] != str.data[i])
		{
			return i;
		}
	}
	return min_len;
}

void StrView::TrimLeft()
{
	const char* ptr = data;

	size_t skip;
	for(skip = 0; skip < size; ++skip, ++ptr)
	{
		if(*ptr > ' ') break;
	}
	data += skip;
	size -= skip;
}

void StrView::TrimRight()
{
	const char* last = data + size - 1;

	size_t skip;
	for(skip = 0; skip < size; ++skip, --last)
	{
		if(*last > ' ') break;
	}

	size -= skip;
}

std::string TrimLeft(const std::string& value)
{
	const char* ptr = value.data();

	size_t skip;
	for(skip = 0; skip < value.size(); ++skip, ++ptr)
	{
		if(*ptr > ' ') break;
	}

    return value.substr(skip);
}

std::string TrimRight(const std::string& value)
{
	const char* last = value.data() + value.size() - 1;

	size_t skip;
	for(skip = 0; skip < value.size(); ++skip, --last)
	{
		if(*last > ' ') break;
	}

    return value.substr(0, value.size() - skip);
}

} 

