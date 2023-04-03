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

#ifndef __xfutil_strutil_h__
#define __xfutil_strutil_h__

#include <string.h>
#include <string>
#include <cassert>
#include <cstddef>
#include <errno.h>
#include <stdlib.h>
#include "xfutil/types.h"

namespace xfutil
{

static inline byte_t* xmalloc(size_t size)
{
	return (byte_t*)malloc(size);
}
static inline void xfree(void* ptr)
{
	free(ptr);
}

#ifdef __linux__
static inline void StrCpy(char* dst, size_t dst_len, const char* src)
{
	memccpy(dst, src, '\0', dst_len);
	dst[dst_len-1] = '\0';
}
#else
static inline void StrCpy(char* dst, size_t dst_len, const char* src)
{
	_memccpy(dst, src, '\0', dst_len);
	dst[dst_len-1] = '\0';
}
#endif

static inline void StrCpy(char* dst, size_t dst_len, const char* src, size_t src_len)
{
	size_t min_len = MIN(dst_len, src_len);
	memcpy(dst, src, min_len);
	dst[min_len-1] = '\0';
}

/////////////////////////////////////////////////////////
class String
{	
public:
	String()
	{
		m_buf = nullptr;
		m_size = 0;
		m_capacity = 0;
	}

	~String()
	{
		if(m_buf != nullptr)
		{
			xfree(m_buf);
            m_buf = nullptr;
		}
	}
	
public:
	inline char* Data() const 
	{
		return m_buf;
	}
	inline size_t Size() const
	{
		return m_size;
	}
	inline bool Empty() const
	{
		return (m_size == 0);
	}
	char& operator[](size_t idx) 
	{
		assert(idx < m_size);
		return m_buf[idx];
	}

	bool Reserve(size_t size);
	
	bool Resize(size_t size);
		
	bool Assign(const char* data, size_t size)
	{
		m_size = 0;
		return Append(data, size);
	}

	bool Append(const char* data, size_t size);
	
	void Clear()
    {
        m_size = 0;
    }
		
private:
	char* m_buf;
	size_t m_size;
	size_t m_capacity;

private:
	String(const String&) = delete;
	void operator=(const String&) = delete;
};


////////////////////////////////////////////////////////
struct StrView
{
	const char* data;
	size_t size;

public:
	StrView()
	{
		data = "";
		size = 0;
	}

	explicit StrView(const char* data_)
	{
        Set(data_, strlen(data_));
	}

	explicit StrView(const char* data_, size_t size_)
	{
        Set(data_, size_);
	}
	
	explicit StrView(const String& str)
	{
        Set(str.Data(), str.Size());
	}
	explicit StrView(const std::string& str)
	{
        Set(str.data(), str.size());
	}
public:
	inline const char* Data() const 
	{
		return data;
	}

	inline size_t Size() const 
	{ 
		return size;
	}

	inline bool Empty() const 
	{ 
		return size == 0; 
	}
    
	char operator[](size_t idx) const
	{
		assert(idx < size);
		return data[idx];
	}

	inline void Clear() 
	{
		data = "";
		size = 0;
	}
	inline void Set(const char* data_, size_t size_)
	{
		data = data_;
		size = size_;
	}
	inline void Set(const char* data_)
	{
		data = data_;
		size = strlen(data_);
	}
	inline void Set(const String& str)
	{
		data = str.Data();
		size = str.Size();
	}
	inline void Skip(size_t n) 
	{
		assert(n <= size);
		data += n;
		size -= n;
	}

	int Compare(const StrView& dst) const;
	
	bool operator<(const StrView& dst) const
	{
		return Compare(dst) < 0;
	}
	bool operator==(const StrView& dst) const
	{
		return (size == dst.size) && (memcmp(data, dst.data, dst.size) == 0);
	}
	bool operator!=(const StrView& dst) const
	{
		return (size != dst.size) || (memcmp(data, dst.data, dst.size) != 0);
	}
	
	bool StartWith(const StrView& str) const 
	{
		return ((size >= str.size) && (memcmp(data, str.data, str.size) == 0));
	}
	bool EndWith(const StrView& str) const
	{
		return ((size >= str.size) && (memcmp(data+(size-str.size), str.data, str.size) == 0));
	}
	uint32_t GetPrefixLength(const StrView& str) const;

};


}

#endif

