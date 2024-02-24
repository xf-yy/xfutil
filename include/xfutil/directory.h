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

#ifndef __xfutil_directory_h__
#define __xfutil_directory_h__

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "xfutil/strutil.h"

namespace xfutil 
{

class Directory
{
public:	
	//创建目录，必须保证父目录存在
	static bool Create(const char* path);

	//递归创建目录
	static bool Create_r(const char* path);

	//递归移除目录
	static bool Remove(const char* path);
	static inline bool Rename(const char* old_path, const char* new_path)
	{
		return rename(old_path, new_path) == 0;
	}
	static inline bool Exist(const char* path)
	{
		struct stat st;
		return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
	}
	
};


}  

#endif


