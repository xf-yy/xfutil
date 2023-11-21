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

#include <fnmatch.h>
#include <dirent.h>
#include <string.h>
#include "xfutil/strutil.h"
#include "xfutil/directory.h"
#include "xfutil/path.h"


namespace xfutil 
{

bool Directory::Create(const char* path)
{
	int ret = 0;
	if(!Exist(path))
	{
		mode_t old_mask = umask(0);
		ret = mkdir(path, 0644);
		umask(old_mask);
	}
	return (ret == 0);
}

static bool Create_r(const char* path)
{
	//可能要修改内容
	char* slash = strchr((char*)path+1, '/');
	while(slash != nullptr)
	{
		*slash = '\0';
		
		if(!Directory::Exist(path))
		{
			if(mkdir(path, 0644) != 0)
			{
				return false;
			}
		}
		*slash = '/';	//恢复
		slash = strchr(slash+1, '/');
	}
	if(!Directory::Exist(path))
	{
		if(mkdir(path, 0644) != 0)
		{
			return false;
		}
	}
	return true;
}

bool Directory::Create_r(const char* path)
{
	mode_t old_mask = umask(0);
	bool ret = xfutil::Create_r(path);
	umask(old_mask);
	return ret;
}

bool Directory::Remove(const char* path)
{
	DIR* dir = opendir(path);
	if(dir == nullptr)
	{
		return false;
	}
	struct dirent* entry;
	while((entry=readdir(dir)) != nullptr)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}	

		char full_path[MAX_PATH_LEN];
		Path::CombineEx(full_path, sizeof(full_path), path, entry->d_name);
		
		struct stat st;
		if(stat(full_path, &st) != 0)
		{
			closedir(dir);
			return false;
		}	
		if(S_ISDIR(st.st_mode))
		{
			Remove(full_path);
			rmdir(full_path);
		}
		else
		{
			unlink(full_path);
		}
	}
	
	rmdir(path);
	
	closedir(dir);
	return true;
}

bool Directory::List(const char* path, const char* pattern, ListCallback cb, void* arg)
{
	assert(cb != nullptr);

	DIR* dir = opendir(path);
	if(dir == nullptr)
	{
		return false;
	}
	struct dirent* entry;
	while((entry=readdir(dir)) != nullptr)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}
		int ret = fnmatch(pattern, entry->d_name, 0);//PathMatchSpecW
		if(ret == 0)
		{
			cb(path, entry->d_name, arg);
		}
	}
	
	closedir(dir);
	return true;
}

bool Directory::Find(const char* path, const char* pattern, ListCallback cb, void* arg)
{
	assert(cb != nullptr);

	DIR* dir = opendir(path);
	if(dir == nullptr)
	{
		return false;
	}
	struct dirent* entry;
	while((entry=readdir(dir)) != nullptr)
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}
		char full_path[MAX_PATH_LEN];
		Path::CombineEx(full_path, sizeof(full_path), path, entry->d_name);
		
		struct stat st;
		if(stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
		{
			Find(full_path, pattern, cb, arg);
		}
		
		int ret = fnmatch(pattern, entry->d_name, 0);//PathMatchSpecW
		if(ret == 0)
		{			
			cb(full_path, entry->d_name, arg);
		}
	}
	
	closedir(dir);
	return true;
}


}  



