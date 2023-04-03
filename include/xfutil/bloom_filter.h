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

#ifndef __xfutil_bloomfilter_h__
#define __xfutil_bloomfilter_h__

#include <string>
#include <malloc.h>
#include <string.h>
#include <deque>
#include "xfutil/strutil.h"

namespace xfutil
{

class BloomFilter
{
public:
	explicit BloomFilter(uint32_t bitnum_perkey);
	~BloomFilter()
	{}
	
public:
	/**申请空间，返回数据指针*/
	bool Create(const uint32_t* hash_ptr, uint32_t count);
	bool Create(std::deque<uint32_t>& hashs);
	
	/**申请空间，并写入数据，返回数据指针*/
	void Attach(const std::string& data)
	{
		m_data = data;
	}

	/**校验布隆值*/
	bool Check(uint32_t hc);

	const std::string& Data()
	{
		return m_data;
	}
		
private:	
	const uint32_t m_bitnum_perkey;
	uint32_t m_k_num;
	std::string m_data;

private:
	BloomFilter(const BloomFilter&) = delete;
	BloomFilter& operator=(const BloomFilter&) = delete;
};

}

#endif

