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

#include <unistd.h>
#ifdef __linux__
#include <sys/uio.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "xfutil/buffer.h"
#include "xfutil/file.h"

namespace xfutil 
{

const int File::OF_READONLY = O_RDONLY;
const int File::OF_WRITEONLY = O_WRONLY;
const int File::OF_READWRITE = O_RDWR;
const int File::OF_CREATE = O_CREAT;			//不存在时则创建
const int File::OF_APPEND = O_APPEND;			//追加文件写
const int File::OF_TRUNCATE = O_TRUNC;		//文件存在+写方式时，文件截断为0
const int File::OF_DIRECT = O_DIRECT;

File::File()
{
	m_fd = INVALID_FD;
}

File::~File()
{
	Close();
}

bool File::Open(const char* file_path, uint32_t flags)
{
    assert(file_path != nullptr && file_path[0] != '\0');
    
	flags |= O_LARGEFILE;

	mode_t old_mask = umask(0);
	m_fd = open(file_path, flags, 0644);
	umask(old_mask);

	return (m_fd != INVALID_FD);
}

void File::Close()
{
    if(m_fd != INVALID_FD)
    {
        close(m_fd);
        m_fd = INVALID_FD;
    }
}

int64_t File::Read(void* buf, size_t buf_size) const
{
    ssize_t rs = read(m_fd, buf, buf_size);
    while(rs == -1 && LastError == EINTR)
    {
        rs = read(m_fd, buf, buf_size);
    }
    return rs;
}
int64_t File::Read(iovec_t* iov, int iov_cnt) const
{
    ssize_t rs = readv(m_fd, iov, iov_cnt);
    while(rs == -1 && LastError == EINTR)
    {
        rs = readv(m_fd, iov, iov_cnt);
    }
    return rs;
}
int64_t File::Read(uint64_t offset, void* buf, size_t buf_size) const
{
    ssize_t rs = pread(m_fd, buf, buf_size, offset);
    while(rs == -1 && LastError == EINTR)
    {
        rs = pread(m_fd, buf, buf_size, offset);
    }
    return rs;
}
int64_t File::Read(uint64_t offset, iovec_t* iov, int iov_cnt) const
{
    ssize_t rs = preadv(m_fd, iov, iov_cnt, offset);
    while(rs == -1 && LastError == EINTR)
    {
        rs = preadv(m_fd, iov, iov_cnt, offset);
    }
    return rs;
}

int64_t File::Write(const void* buf, size_t buf_size)
{
    ssize_t ws = write(m_fd, buf, buf_size);
    while(ws == -1 && LastError == EINTR)
    {
        ws = write(m_fd, buf, buf_size);
    }
    return ws;
}
int64_t File::Write(iovec_t* iov, int iov_cnt)
{
    ssize_t ws = writev(m_fd, iov, iov_cnt);
    while(ws == -1 && LastError == EINTR)
    {
        ws = writev(m_fd, iov, iov_cnt);
    }
    return ws;
}	
int64_t File::Write(uint64_t offset, const void* buf, size_t buf_size)
{
    ssize_t ws = pwrite(m_fd, buf, buf_size, offset);
    while(ws == -1 && LastError == EINTR)
    {
        ws = pwrite(m_fd, buf, buf_size, offset);
    }
    return ws;
}
	
int64_t File::Write(uint64_t offset, iovec_t* iov, int iov_cnt)
{
    ssize_t ws = pwritev(m_fd, iov, iov_cnt, offset);
    while(ws == -1 && LastError == EINTR)
    {
        ws = pwritev(m_fd, iov, iov_cnt, offset);
    }
    return ws;
}    
//SetFilePointerEx() followed by SetEndOfFile()
	
bool File::Copy(const char *src_filepath, const char *dst_filepath, bool sync/* = false*/)
{
    File src_file;
    if(!src_file.Open(src_filepath, OF_READONLY))
    {
        return false;
    }

    File dst_file;
    if(!dst_file.Open(dst_filepath, OF_CREATE|OF_WRITEONLY|OF_TRUNCATE))
    {
        return false;
    }

    //1MiB为单位
    constexpr int BUF_SIZE = 1024*1024;
    byte_t* buf = xmalloc(BUF_SIZE);
    BufferGuard buf_guard(buf);

    for(;;)
    {
        int64_t read_size = src_file.Read(buf, BUF_SIZE);
        if(read_size < 0)
        {
            return false;
        }
        else if(read_size == 0)
        {
            break;
        }
        if(dst_file.Write(buf, read_size) != read_size)
        {
            return false;
        }
    }
    
    if(sync)
    {
        dst_file.Sync();
    }
    return true;
}	
} 



