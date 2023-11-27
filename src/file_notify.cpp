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
#include "xfutil/file_notify.h"
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

const int FileNotify::FE_CREATE = IN_CREATE;
const int FileNotify::FE_OPEN = IN_OPEN;
const int FileNotify::FE_CLOSE = IN_CLOSE;
const int FileNotify::FE_DELETE = IN_DELETE|IN_MOVED_FROM;
const int FileNotify::FE_RENAME = IN_MOVED_TO;
const int FileNotify::FE_MODIFY = IN_MODIFY;

FileNotify::FileNotify(HandleNotifyCallback cb, void* arg) : m_cb(cb), m_arg(arg)
{
	m_ifd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
}

FileNotify::~FileNotify()
{
	RemoveAll();
	if(m_ifd != INVALID_FD)
	{
		close(m_ifd);
	}
}

bool FileNotify::Add(const std::string& path, uint32_t events, bool created_if_missing/* = true*/)
{
	assert(m_ifd == INVALID_FD);
	if(m_ifd != INVALID_FD)
	{
		return false;
	}

	if(!Directory::Exist(path.c_str()))
	{
		if(!created_if_missing)
		{
			return false;
		}
		if(!Directory::Create(path.c_str()))
		{
			return false;
		}
	}

    std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_wfds.find(path);
    if(it != m_wfds.end())
    {
        return false;
    }

	int wfd = inotify_add_watch(m_ifd, path.c_str(), events);
	if(wfd == INVALID_FD)
    {
        return false;
    }
    m_wfds[path] = wfd;
    m_paths[wfd] = path;
    return true;
}

void FileNotify::Remove(const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_wfds.find(path);
    if(it != m_wfds.end())
    {
        inotify_rm_watch (m_ifd, it->second);
        m_paths.erase(it->second);
        m_wfds.erase(it);
    }
}

void FileNotify::RemoveAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for(auto it = m_wfds.begin(); it != m_wfds.end(); ++it)
    {
        inotify_rm_watch (m_ifd, it->second);
    }
    m_wfds.clear();
    m_paths.clear();
}

int FileNotify::Select(int fd, int events, uint32_t timeout_ms)
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

std::string FileNotify::GetPath(int wfd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_paths.find(wfd);
    if(it != m_paths.end())
    {
        return it->second;
    }
    return std::string();
}

void FileNotify::Handle(char* buf, char* end)
{
    char path[MAX_PATH_LEN];
    while(buf < end)
    {
        struct inotify_event* ev = (struct inotify_event*)buf;
        buf += sizeof(struct inotify_event) + ev->len;

        //根据ev->wd获取其path
        std::string wpath = GetPath(ev->wd);
        Path::Combine(path, sizeof(path), wpath.c_str(), ev->name);   

        m_cb(path, ev->mask, m_arg);     
    }
}

int FileNotify::Read(uint32_t timeout_ms)
{
    char buf[IBUF_SIZE];

    for(;;)
    {
        ssize_t rsize = read(m_ifd, buf, IBUF_SIZE);
        if(rsize > 0)
        {
            Handle(buf, buf+rsize);
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


