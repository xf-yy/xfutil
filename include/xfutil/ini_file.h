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

#ifndef __xfutil_ini_file_h__
#define __xfutil_ini_file_h__

#include <memory>
#include <map>
#include <ctime>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/uio.h>
#include "xfutil/strutil.h"

namespace xfutil 
{

class IniSection
{
public:
	void Set(const std::string& key, const std::string& value)
	{
		m_values[key] = value;
	}
	const std::string* Get(const std::string& key) const
	{
		auto iter = m_values.find(key);
		if(iter != m_values.end())
		{
			return &iter->second;
		}
		return nullptr;
	}
	const std::map<std::string, std::string>& GetValues() const
	{
		return m_values;
	}
private:	
	std::map<std::string, std::string> m_values;
};

class IniFileReader
{
public:
	IniFileReader()
	{
		m_curr_section = nullptr;
	}
	~IniFileReader(){}
	
public:
	//0成功，其他失败	
	int Parse(const char* file_path);

	const IniSection* GetSection() const
	{
		return GetSection("");
	}
	const IniSection* GetSection(const std::string& section) const
	{
		auto iter = m_sections.find(section);
		if(iter != m_sections.end())
		{
			return &iter->second;
		}
		return nullptr;
	}
	const std::map<std::string, IniSection>& GetSections() const
	{
		return m_sections;
	}

private:
	void ParseLine(const std::string& line);	

	static std::string TrimValue(const std::string& value);	//去引号和注释

private:
	std::map<std::string, IniSection> m_sections;
	IniSection* m_curr_section;

private:
	IniFileReader(const IniFileReader&) = delete;
	IniFileReader& operator=(const IniFileReader&) = delete;
};

} 

#endif


