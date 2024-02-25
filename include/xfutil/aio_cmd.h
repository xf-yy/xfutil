#ifndef __xfutil_aio_command_h__
#define __xfutil_aio_command_h__

#include <vector>
#include <sys/uio.h>
#include "xfutil/types.h"

namespace xfutil
{
namespace aio
{

enum
{
	CMD_READ,
	CMD_WRITE,
	CMD_READV,
	CMD_WRITEV,

	CMD_MIN_PIO,

	CMD_PREAD,
	CMD_PWRITE,
	CMD_PREADV,
	CMD_PWRITEV,
	CMD_READAHEAD,
	
	CMD_FSYNC,
	CMD_FDATASYNC,
};

struct Command
{	
	inline void Read(int fd, void *buf, size_t size);
	inline void Read(int fd, uint64_t pos, void *buf, size_t size);
	inline void Read(int fd, iovec *iobuf, int iovcnt);
	inline void Read(int fd, uint64_t pos, iovec *iobuf, int iovcnt);
	inline void ReadAhead(int fd, uint64_t pos, size_t size);

	inline void Write(int fd, void *buf, size_t size);
	inline void Write(int fd, uint64_t pos, void *buf, size_t size);
	inline void Write(int fd, iovec *iobuf, int iovcnt);
	inline void Write(int fd, uint64_t pos, iovec *iobuf, int iovcnt);

	inline void Sync(int fd);
	inline void DataSync(int fd);

private:
	void Set(int cmd, int fd, void *buf, size_t size);
	void Set(int cmd, int fd, uint64_t pos, void *buf, size_t size);

public:
	int cmd;
	int fd;
	uint64_t pos;
	void *buf;			//iovec
	size_t size;		//iobuf_cnt

	ssize_t ret;
	int error;			//错误码
};

typedef void (*CompleteCallback)(const std::vector<Command>& cmds, uint32_t cmd_fail_cnt, int64_t elapsed_time, void* arg);

void Command::Read(int fd, void *buf, size_t size)
{
	Set(CMD_READ, fd, buf, size);
}

void Command::Write(int fd, void *buf, size_t size)
{
	Set(CMD_WRITE, fd, buf, size);
}
void Command::Read(int fd, uint64_t pos, void *buf, size_t size)
{
	Set(CMD_PREAD, fd, pos, buf, size);
}
void Command::Write(int fd, uint64_t pos, void *buf, size_t size)
{
	Set(CMD_PWRITE, fd, pos, buf, size);
}
void Command::Read(int fd, iovec *iobuf, int iovcnt)
{
	Set(CMD_READV, fd, iobuf, iovcnt);
}
void Command::Write(int fd, iovec *iobuf, int iovcnt)
{
	Set(CMD_WRITEV, fd, iobuf, iovcnt);

}
void Command::Read(int fd, uint64_t pos, iovec *iobuf, int iovcnt)
{
	Set(CMD_PREADV, fd, pos, iobuf, iovcnt);
}
void Command::Write(int fd, uint64_t pos, iovec *iobuf, int iovcnt)
{
	Set(CMD_PWRITEV, fd, pos, iobuf, iovcnt);
}
void Command::ReadAhead(int fd, uint64_t pos, size_t size)
{
	Set(CMD_READAHEAD, fd, pos, nullptr, size);
}
void Command::Sync(int fd)
{
	Set(CMD_FSYNC, fd, 0, nullptr, 0);
}
void Command::DataSync(int fd)
{
	Set(CMD_FDATASYNC, fd, 0, nullptr, 0);
}




}
}

#endif
