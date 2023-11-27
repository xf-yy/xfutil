#ifndef __xfutil_aio_h__
#define __xfutil_aio_h__

#include <atomic>
#include "xfutil/aio_cmd.h"

namespace xfutil
{
namespace aio
{

//启动aio
//io_threadnum: 不带offset的io线程数
//pio_threadnum: 带offset的io线程数
int Start(int io_threadnum = -1, int pio_threadnum = -2);

//停止aio
int Stop();

//提交io操作
uint64_t Submit(const std::vector<Command>& cmds, CompleteCallback cb, void *arg = NULL);	
	
}
}

#endif
