#include "aio_request.h"
#include "xfutil/time.h"

namespace xfutil
{
namespace aio
{

Request::Request(const std::vector<Command>& cmds, CompleteCallback cb, void *arg/* = NULL*/)
	: m_cmds(cmds), m_callback(cb), m_arg(arg), m_create_time(GetTickMilliTime()),
	  m_cmd_fail_cnt(0), m_completed_cnt(0)
{
	m_completed_time = -1;
}






}
}



