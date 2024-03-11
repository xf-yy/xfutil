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

#ifndef __xfutil_hash_h__
#define __xfutil_hash_h__

#include "xfutil/strutil.h"

namespace xfutil 
{

uint32_t Hash32(const byte_t* data, size_t len, uint32_t seed = 4118054813);

uint32_t Hash32(uint32_t value, uint32_t seed = 4118054813);
uint32_t Hash32(uint64_t value, uint32_t seed = 4118054813);

}

#endif

