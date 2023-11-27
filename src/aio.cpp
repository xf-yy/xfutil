#include <fcntl.h>

#include "xfutil/aio.h"
#include "xfutil/thread.h"
#include "xfutil/queue.h"
#include "aio_request.h"
#include "xfutil/sysinfo.h"

namespace xfutil
{
namespace aio
{

enum AioState
{
	AIO_STOPPED = 0,
	AIO_STARTING,
	AIO_STARTED,
	AIO_STOPPING,
};

static std::atomic<AioState> s_state(AIO_STOPPED);

static std::atomic<uint64_t> s_reqid(1);
static BlockingQueue<RequestEx>* s_io_request_queues = nullptr;
static xfutil::ThreadGroup s_io_thread_group;

static BlockingQueue<RequestEx> s_pio_request_queue;
static xfutil::ThreadGroup s_pio_thread_group;

static void ProcessRequest(RequestEx& req_ex)
{
	RequestPtr& request = req_ex.request;
	Command& cmd = request->m_cmds[req_ex.cmd_idx];

	switch(cmd.cmd)
	{
	case CMD_READ:
		cmd.ret = ::read(cmd.fd, cmd.buf, cmd.size);
		break;
	case CMD_WRITE:
		cmd.ret = ::write(cmd.fd, cmd.buf, cmd.size);
		break;
	case CMD_READV:
		cmd.ret = ::readv(cmd.fd, (iovec *)cmd.buf, (int)cmd.size);
		break;
	case CMD_WRITEV:
		cmd.ret = ::writev(cmd.fd, (iovec *)cmd.buf, (int)cmd.size);
		break;	

	case CMD_PREAD:
		cmd.ret = ::pread(cmd.fd, cmd.buf, cmd.size, cmd.pos);
		break;
	case CMD_PWRITE:
		cmd.ret = ::pwrite(cmd.fd, cmd.buf, cmd.size, cmd.pos);
		break;
	case CMD_PREADV:
		cmd.ret = ::preadv(cmd.fd, (iovec *)cmd.buf, (int)cmd.size, cmd.pos);
		break;
	case CMD_PWRITEV:
		cmd.ret = ::pwritev(cmd.fd, (iovec *)cmd.buf, (int)cmd.size, cmd.pos);
		break;
	case CMD_READAHEAD:
		cmd.ret = ::readahead(cmd.fd, cmd.pos, cmd.size);
		break;
	case CMD_FSYNC:
		cmd.ret = ::fsync(cmd.fd);
		break;
	case CMD_FDATASYNC:
		cmd.ret = ::fdatasync(cmd.fd);
		break;
	
	default:
		cmd.ret = -1;
		LastError = EINVAL;
		break;
	}
	if(cmd.ret == -1)
	{
		cmd.error = LastError;
		request->m_cmd_fail_cnt.fetch_add(1);
	}
	
	uint32_t prev_cnt = request->m_completed_cnt.fetch_add(1);
	if(prev_cnt == request->m_cmds.size()-1)
	{
		request->m_callback(request->m_cmds, request->m_cmd_fail_cnt.load(), request->ElapsedTime(), request->m_arg);
	}
}

static void IOProcThreadFunc(int index, void* arg)
{
	for(;;)
	{
		RequestEx req_ex;
		s_io_request_queues[index].Pop(req_ex);
		if(!req_ex.request)
			break;
		ProcessRequest(req_ex);
	}
	
}

static void PIOProcThreadFunc(int index, void* arg)
{
	for(;;)
	{
		RequestEx req_ex;
		s_pio_request_queue.Pop(req_ex);
		if(!req_ex.request)
			break;
		ProcessRequest(req_ex);
	}
}


int Start(int io_threadnum/* = -1*/, int pio_threadnum/* = -2*/)
{
	AioState exp_state = AIO_STOPPED;
	if(!s_state.compare_exchange_strong(exp_state, AIO_STARTING))
	{
		return -1;
	}

	if(io_threadnum < 0)
	{
		io_threadnum = -io_threadnum * SysInfo::GetCpuNum();
	}
	s_io_request_queues = new BlockingQueue<RequestEx>[io_threadnum];
	s_io_thread_group.Start(io_threadnum, IOProcThreadFunc);

	if(pio_threadnum < 0)
	{
		pio_threadnum = -pio_threadnum * SysInfo::GetCpuNum();
	}
	s_pio_thread_group.Start(pio_threadnum, PIOProcThreadFunc);

	s_state.store(AIO_STARTED);
	return 0;
}

int Stop()
{
	AioState exp_state = AIO_STARTED;
	if(!s_state.compare_exchange_strong(exp_state, AIO_STOPPING))
	{
		return -1;
	}

	RequestEx empty_req_ex;
	for(size_t i = 0; i < s_io_thread_group.Size(); ++i)
	{
		s_io_request_queues[i].Push(empty_req_ex);
	}
	s_io_thread_group.Join();
	delete[] s_io_request_queues;
	s_io_request_queues = nullptr;

	for(size_t i = 0; i < s_pio_thread_group.Size(); ++i)
	{
		s_pio_request_queue.Push(empty_req_ex);
	}
	s_pio_thread_group.Join();

	s_state.store(AIO_STOPPED);
	return 0;
}

uint64_t Submit(std::vector<Command>& cmds, CompleteCallback cb, void *arg/* = NULL*/)
{
	if(s_state.load() != AIO_STARTED)
	{
		return 0;
	}
	RequestPtr request = NewRequest(cmds, cb, arg);
	RequestEx req_ex(request);

	//有多少cmd，则压入多少个请求到队列里
	for(size_t i = 0; i < cmds.size(); ++i)
	{
		req_ex.cmd_idx = i;

		Command& cmd = cmds[i];
		if(cmd.cmd < CMD_MIN_PIO)
		{
			size_t queue_idx = cmd.fd % s_io_thread_group.Size();
			s_io_request_queues[queue_idx].Push(req_ex);
		}
		else
		{
			s_pio_request_queue.Push(req_ex);
		}
	}
	return s_reqid.fetch_add(1);
}


}
}



