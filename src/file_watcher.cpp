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

#include "xfutil/file.h"
#include "xfutil/file_watcher.h"
#include "xfutil/strutil.h"
#include "xfutil/buffer.h"
#ifdef __linux__
#include <sys/inotify.h>
#include <unistd.h>
#endif
#include <poll.h>
#include <fcntl.h>

enum {IBUF_SIZE = 4096};

namespace xfutil 
{

const int FileWatcher::FE_CREATE = IN_CREATE;
const int FileWatcher::FE_OPEN = IN_OPEN;
const int FileWatcher::FE_CLOSE = IN_CLOSE;
const int FileWatcher::FE_DELETE = IN_DELETE|IN_MOVED_FROM;
const int FileWatcher::FE_RENAME = IN_MOVED_TO;
const int FileWatcher::FE_MODIFY = IN_MODIFY;

FileWatcher::FileWatcher()
{
	m_ifd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
}

FileWatcher::~FileWatcher()
{
	RemoveAll();
	if(m_ifd != INVALID_FILEID)
	{
		close(m_ifd);
	}
}

bool FileWatcher::Add(const std::string& path, uint32_t events)
{
	assert(m_ifd == INVALID_FILEID);
	if(m_ifd != INVALID_FILEID)
	{
		return false;
	}

	if(!Directory::Exist(path.c_str()))
	{
		return false;
	}

    std::lock_guard<std::mutex> lock(m_mutex);

	int wfd = inotify_add_watch(m_ifd, path.c_str(), events);
	if(wfd == INVALID_FILEID)
    {
        return false;
    }
    m_paths[wfd] = path;
    return true;
}

void FileWatcher::RemoveAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto it = m_paths.begin(); it != m_paths.end(); ++it)
    {
        inotify_rm_watch (m_ifd, it->first);
    }
    m_paths.clear();
}

int FileWatcher::Select(int fd, int events, uint32_t timeout_ms)
{
    int ret;
    for(;;)
    {
        struct pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = events;      
        fds[0].revents = 0;

        ret = poll(fds, 1, timeout_ms);
        if(ret > 0)
        {
            if(fds[0].revents & POLLERR)
            {
                return -1;
            }
            else
            {
                assert(fds[0].revents > 0);
                return fds[0].revents;
            }
        }
        else if(ret == 0)
        {
            //LastError = ERR_TIMEOUT;
            break;
        }
        else if(LastError != EINTR)
        {
            break;
        }
    }
    return ret;
}

bool FileWatcher::GetFilePath(int wfd, const char* file_name, char* file_path, uint32_t file_path_size)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_paths.find(wfd);
    if(it != m_paths.end())
    {
        Path::Combine(file_path, file_path_size, it->second.c_str(), file_name);   
        return true;
    }

    return false;
}

void FileWatcher::HandleEvent(char* buf, char* end, EventCallback cb, void* arg/* = nullptr*/)
{
    char path[MAX_PATH_LEN];
    while(buf < end)
    {
        struct inotify_event* ev = (struct inotify_event*)buf;
        buf += sizeof(struct inotify_event) + ev->len;

        //根据ev->wd获取其path
        if(GetFilePath(ev->wd, ev->name, path, sizeof(path)))
        {
            cb(path, ev->mask, arg);     
        }
    }
}

int FileWatcher::Read(uint32_t timeout_ms, EventCallback cb, void* arg/* = nullptr*/)
{
    assert(cb != nullptr);

    char buf[IBUF_SIZE];

    for(;;)
    {
        ssize_t rsize = read(m_ifd, buf, IBUF_SIZE);
        if(rsize > 0)
        {
            HandleEvent(buf, buf+rsize, cb, arg);
        }
        else if(rsize < 0)
        {
            if(LastError == EAGAIN)
            {
                int events = Select(m_ifd, POLLIN | POLLPRI, timeout_ms);
                if(events <= 0)
                {
                    return events;
                }
            }
            else if(LastError != EINTR)
            {
                return -1;
            }
        }
    }
    return 1;

}
		

}  


