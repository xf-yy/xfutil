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

#ifndef __xfutil_path_h__
#define __xfutil_path_h__

#include <stdio.h>
#include "xfutil/strutil.h"
#include <unistd.h>

namespace xfutil 
{

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN	256	//包含结束符'\0'
#endif

#define PATH_SEPARATOR	'/'

class Path
{
public:
	static const char* GetFileName(const char* path)
    {
        const char* name = strrchr(path, '/');
        return name == nullptr ? path : name+1;
    }
	//static const char* GetExtName(const char* path);
	//static int GetDirPath(const char* path, char dir_path[MAX_PATH_LEN]);

	static void CombineEx(char* path, size_t path_size, const char* path1, const char* path2)
	{
		assert(path != nullptr);
		assert(path1 != nullptr && path1[0] != '\0');
		assert(path2 != nullptr && path2[0] != '/');
		if(path1[strlen(path1) - 1] == '/')
		{
			snprintf(path, path_size, "%s%s", path1, path2);
		}
		else
		{
			snprintf(path, path_size, "%s/%s", path1, path2);
		}
	}
	static void Combine(char* path, size_t path_size, const char* path1, const char* path2)
	{
		assert(path != nullptr);
		assert(path1 != nullptr && path1[0] != '\0');
		assert(path2 != nullptr && path2[0] != '/');
		snprintf(path, path_size, "%s/%s", path1, path2);
	}
    //创建硬链接
	static bool HardLink(const char* exist_path, const char* new_path)
    {
        return (link(exist_path, new_path) == 0);
    }
    //创建软链接
    static bool SymLink(const char* exist_path, const char* new_path)
    {
        return (symlink(exist_path, new_path) == 0);
    }

	#if 0
	static AddBackslash();
	static RemoveBackslash();
	static RemoveExtName();
	static AddExtName();
	static RemoveFileName();
	static AddFileName();
	static Append();
	static GetTempDirPath(char* buf, int buf_len);
	static GetWorkPath(char* buf, int buf_len);
	static SetWorkPath(char* path);
	#endif
	
private:	
	Path(const Path&) = delete;
	Path& operator=(const Path&) = delete;
};

}

#endif


