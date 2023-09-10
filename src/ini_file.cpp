/*************************************************************************
Copyright (C) 2022 The xfdb Authors. All rights reserved.

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

#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "xfutil/buffer.h"
#include "xfutil/ini_file.h"

namespace xfutil 
{

int IniFileReader::Parse(const char* file_path)
{
    std::ifstream fin;
    fin.open(file_path, std::ios::in);
    if(!fin.is_open())
    {
        return 1;
    }
    while (!fin.eof())
    {
        std::string line;
        std::getline(fin, line);

        ParseLine(line);
    }
    fin.close();

    return 0;
}


std::string IniFileReader::TrimValue(const std::string& value)
{
    std::string new_value = TrimLeft(value);
    if(new_value.empty())
    {
        return std::string();
    }

    if(new_value[0] == '\"')
    {
        auto pos = new_value.find('\"', 1);
        if(pos != std::string::npos)
        {
            return new_value.substr(1, pos-1);
        }
        //错误格式？
    }
    else if(new_value[0] == '\'')
    {
        auto pos = new_value.find('\'', 1);
        if(pos != std::string::npos)
        {
            return new_value.substr(1, pos-1);
        }
        //错误格式
    }
    else
    {
        //剔除注释内容
        auto pos = new_value.find_first_of(";#");
        if(pos != std::string::npos)
        {
            new_value = new_value.substr(0, pos);
        }
    }
    return TrimRight(new_value);

}

void IniFileReader::ParseLine(const std::string& line)
{
    std::string new_line = TrimLeft(line);

    //空行
    if(new_line.empty())
    {
        return;
    }
    //注释行
    if(new_line[0] == '#' || new_line[0] == ';')
    {
        return;
    }

    //section行
    if(new_line[0] == '[')
    {
        auto pos = new_line.find(']');
        if(pos != std::string::npos)
        {
            std::string section = new_line.substr(1, pos-1);
            m_curr_section = &m_sections[section];
        }
        return;
    }
    //key=value行
    auto pos = new_line.find('=');
    if(pos == std::string::npos)    //没有key，value值
    {
        return;
    }
    auto key = new_line.substr(0, pos);
    auto value = new_line.substr(pos+1);
    key = TrimRight(key);
    if(!value.empty())
    {
        value = TrimValue(value);
    }

    //可能是没有section名
    if(m_curr_section == nullptr)
    {
        m_curr_section = &m_sections[""];
    }
    m_curr_section->Set(key, value);
}

} 



