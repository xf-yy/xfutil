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

#ifndef __xfutil_logger_impl_h__
#define __xfutil_logger_impl_h__

#include <atomic>
#include <stdio.h>
#include <stdarg.h>
#include "xfutil/path.h"
#include "xfutil/logger.h"
#include "xfutil/queue.h"
#include "xfutil/thread.h"
#include "xfutil/file.h"
#include "xfutil/strutil.h"
#include "xfutil/buffer.h"
#include "xfutil/block_pool.h"

namespace xfutil 
{

struct LogData
{
    void* buf;
    uint32_t len;
};

class LoggerImpl
{
public:
	LoggerImpl();
    ~LoggerImpl()
    {
        Close();
    }
public:
	bool Start(const LogConfig& conf);
	void Close();
	
	void LogMsg(LogLevel level, const char *file_name, const char* func_name, int line_num, const char *format, va_list args);

private:
	bool ParseConf(const char *conf_file);

	bool Reopen();
    void Rename();
	bool Write(const LogData& data);

	static void LogThread(void* arg);

private:
	LogConfig m_conf;
    volatile bool m_started;

	File m_logfile;
    uint64_t m_filesize;
	BlockingQueue<LogData> m_data_queue;
	Thread m_thread;
    BlockPool m_pool;
};


}

#endif
