#include "xfutil/aio.h"

namespace xfutil
{

namespace aio
{

void Command::Set(int cmd, int fd, void *buf, size_t size)
{
	this->cmd = cmd;
	this->fd = fd;
	this->buf = buf;
	this->size = size;

	this->ret = -1;
	this->error = 0;
}

void Command::Set(int cmd, int fd, uint64_t pos, void *buf, size_t size)
{
	this->cmd = cmd;
	this->fd = fd;
	this->pos = pos;
	this->buf = buf;
	this->size = size;

	this->ret = -1;
	this->error = 0;
}


}
}



