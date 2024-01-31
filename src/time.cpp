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

//hh:mm:ss
uint32_t ParseTime(char* buf, int buf_len)
{
    if(buf_len != 8 || buf[2] != ':' || buf[5] != ':')
        return (uint32_t)-1;

    uint32_t h = strtol(buf, nullptr, 10);
    uint32_t m = strtol(buf+3, nullptr, 10);        
    uint32_t s = strtol(buf+6, nullptr, 10);

    return (h<<12) | (m<<6) | s;      
}
//yyyy-mm-dd
uint32_t ParseDate(char* buf, int buf_len)
{
    if(buf_len != 10 || buf[4] != '-' || buf[7] != '-')
        return (uint32_t)-1;

    uint32_t y = strtol(buf, nullptr, 10);
    uint32_t m = strtol(buf+5, nullptr, 10);        
    uint32_t d = strtol(buf+8, nullptr, 10);

    return (y<<9) | (m<<5) | d;          
}
//yyyy-mm-dd hh:mm:ss
uint64_t ParseDateTime(char* buf, int buf_len)
{
    if(buf_len != 19 || buf[10] != ' ')
        return (uint64_t)-1;

    uint32_t d = ParseDate(buf, 10);
    if(d == (uint32_t)-1)
        return (uint64_t)-1;

    uint32_t t = ParseTime(buf+11, 8);
    if(t == (uint32_t)-1)
        return (uint64_t)-1;

    return ((uint64_t)d << 32) | t;
}


int FormatTime(char* buf, int buf_len, uint32_t time_)
{
    if(buf_len < 8)
        return -1;
    uint32_t h = time_ >> 12;
    uint32_t m = (time_ >> 6) & 63;
    uint32_t s = time_ & 63;

    return snprintf(buf, buf_len, "%02u:%02u:%02u", h,m,s);
}

int FormatDate(char* buf, int buf_len, uint32_t date_)
{
    if(buf_len < 10)
        return -1;
    uint32_t y = date_ >> 9;
    uint32_t m = (date_ >> 5) & 16;
    uint32_t d = date_ & 31;

    return snprintf(buf, buf_len, "%04u-%02u-%02u", y,m,d);
}

int FormatDateTime(char* buf, int buf_len, uint64_t dt)
{
    if(buf_len < 19)
        return -1;

    uint32_t date_ = dt>>32;
    uint32_t time_ = (uint32_t)dt;

    int len1 = FormatDate(buf, 10, date_);
    buf[10] = ' ';
    int len2 = FormatTime(buf+11, 8, time_);
    return len1 + len2;
}


}


