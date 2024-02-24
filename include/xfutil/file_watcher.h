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

#ifndef __xfutil_file_watcher_h__
#define __xfutil_file_watcher_h__

#include <string>
#include <mutex>
#include <map>
#include "xfutil/strutil.h"
#include "path.h"
#include "directory.h"

namespace xfutil 
{


class FileWatcher
{
public:
	FileWatcher();
	~FileWatcher();

public:
	static const int FE_CREATE;
  	static const int FE_OPEN;
  	static const int FE_CLOSE;
	static const int FE_DELETE;
	static const int FE_RENAME;
	static const int FE_MODIFY;

	typedef void (*EventCallback)(const char* file_path, uint32_t events, void* arg);
	
public: 
	bool Add(const std::string& path, uint32_t events);

    int Read(uint32_t timeout_ms, EventCallback cb, void* arg = nullptr);

private:
    void RemoveAll();
    void HandleEvent(char* buf, char* end, EventCallback cb, void* arg = nullptr);
    bool GetFilePath(int wfd, const char* file_name, char* file_path, uint32_t file_path_size);

    //-1表示错误，0表示超时，正数表示事件
    static int Select(int fd, int events, uint32_t timeout_ms);

private:
	fileid_t m_ifd;

	std::mutex m_mutex;
    std::map<int, std::string> m_paths;
};


}

#endif


