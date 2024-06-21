#ifndef __xfutil_aio_request_h__
#define __xfutil_aio_request_h__

#include <atomic>
#include <vector>
#include "xfutil/aio.h"

namespace xfutil
{
namespace aio
{

class Request
{
public:
	Request(const std::vector<Command>& cmds, CompleteCallback cb, void *arg = NULL);
	~Request()
	{
	}

	//单位: 毫秒
	inline int64_t ElapsedTime()
	{
		return (m_completed_time - m_create_time);
	}	

public:	
	std::vector<Command> m_cmds;
	const CompleteCallback m_callback;
	void *m_arg;
	const int64_t m_create_time;

	std::atomic<uint32_t> m_cmd_fail_cnt;
	std::atomic<uint32_t> m_completed_cnt;
	int64_t m_completed_time;				//-1表示未结束

private:
	Request(const Request&) = delete;
	Request& operator=(const Request&) = delete;	
};

typedef std::shared_ptr<Request> RequestPtr;
#define NewRequest 	std::make_shared<Request>

struct RequestEx
{
	RequestEx()
	{
		cmd_idx = 0;
	}
	RequestEx(RequestPtr& req) : request(req)
	{
		cmd_idx = 0;
	}

	RequestPtr request;
	size_t cmd_idx;
};

}


}

#endif
