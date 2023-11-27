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

#ifndef __xfutil_file_h__
#define __xfutil_file_h__

#include <memory>
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

struct FileTime
{
#ifdef _WIN32
	uint64_t create_time;
#else
	uint64_t change_time;
#endif
	uint64_t access_time;
	uint64_t modify_time;
};

#ifdef _WIN32
struct iovec_t
{
    void  *iov_base;   
    size_t iov_len;  
};
#else
typedef struct iovec iovec_t;
#endif

class File
{
public:
	File();
    // 移动构造函数
    File(File&& other)
    {
        m_fd = other.m_fd;
        other.m_fd = INVALID_FD;
    }    
	~File();
	
public:	
	inline fd_t GetFD()
	{
		return m_fd;
	}

    //
	bool Open(const char* file_path, uint32_t flags);
	void Close();
	
	int64_t Read(void* buf, size_t buf_size) const;
	int64_t Read(iovec_t* iov, int iov_cnt) const;
    int64_t Read(uint64_t offset, void* buf, size_t buf_size) const;
	int64_t Read(uint64_t offset, iovec_t* iov, int iov_cnt) const;

	int64_t Write(const void* buf, size_t buf_size);
	int64_t Write(iovec_t* iov, int iov_cnt);
	int64_t Write(uint64_t offset, const void* buf, size_t buf_size);
	int64_t Write(uint64_t offset, iovec_t* iov, int iov_cnt);

	inline bool Sync()
	{
		return fsync(m_fd) == 0;
	}
	inline bool Truncate(size_t new_size)
	{
		return ftruncate(m_fd, new_size) == 0;
	}
	inline bool Allocate(size_t size)
	{
		return fallocate(m_fd, 0, 0, size);
	}
	
	inline bool Seek(int64_t offset)
	{
		return lseek(m_fd, offset, SEEK_SET);
	}
	
	inline int64_t Size() const
	{
		struct stat st;
		return (fstat(m_fd, &st) == 0) ? st.st_size : -1;
	}
	static inline int64_t Size(const char* path)
	{
		struct stat st;
		return (stat(path, &st) == 0) ? st.st_size : -1;
	}
	static inline uint64_t ModifyTime(const char* path)
	{
		struct stat st;
		return (stat(path, &st) == 0) ? st.st_mtime : 0;
	}

	inline bool TryLockRead()
	{
		return flock(m_fd, LOCK_SH|LOCK_NB) == 0;
	}
	inline bool TryLockWrite()
	{
		return flock(m_fd, LOCK_NB|LOCK_EX) == 0;
	}    
	inline bool Unlock()
	{
		return flock(m_fd, LOCK_UN) == 0;
	}
	
	static inline bool Exist(const char *filepath)
	{
		return access(filepath, F_OK) == 0;
	}
	static inline bool Remove(const char *filepath)
	{
		return unlink(filepath) == 0 || LastError == ENOENT;
	}
	static inline bool Rename(const char *src_filepath, const char *dst_filepath)
	{
		return rename(src_filepath, dst_filepath) == 0;
	}
	static bool Copy(const char *src_filepath, const char *dst_filepath, bool sync = false);


public:
	static const int OF_READONLY;
	static const int OF_WRITEONLY;
	static const int OF_READWRITE;
	static const int OF_CREATE;
	static const int OF_APPEND;
	static const int OF_TRUNCATE;
    static const int OF_DIRECT;

protected:
	fd_t m_fd;
	
private:
	File(const File&) = delete;
	File& operator=(const File&) = delete;
};

} 

#endif


