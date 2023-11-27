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

#include <sys/time.h>
#include "xfutil/thread.h"

namespace xfutil
{

//获取当前系统时间，单位秒
uint64_t GetCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return tv.tv_sec;
}

//获取当前系统时间，单位毫秒
uint64_t GetCurrentMilliTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return (tv.tv_sec*1000ULL + tv.tv_usec/1000);
}

//获取系统启动时间，单位毫秒
uint64_t GetTickTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_COARSE , &ts);
    return ts.tv_sec;
}

uint64_t GetTickMilliTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_COARSE , &ts);    //CLOCK_MONOTONIC
    return (ts.tv_sec*1000ULL + ts.tv_nsec/1000000);
}

}


