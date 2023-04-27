/*************************************************************************
Copyright (C) 2022 The xfutil Authors. All rights reserved.

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

#ifndef __xfutil_list_h__
#define __xfutil_list_h__

namespace xfutil
{

struct ListNode 
{
    ListNode* next;
    ListNode* prev;
};
typedef ListNode List;

//初始化节点
static inline void ListInit(List *list)
{
    list->next = list;
    list->prev = list;
}

//判断双链表是否为空
static inline bool ListEmtpy(const List *list)
{
    return list->next == list;
}
static inline ListNode* ListHead(const List *list)
{
    return list->next;
}
static inline ListNode* ListTail(const List *list)
{
    return list->prev;
}

static inline void ListInsert_(ListNode* new_node, ListNode* prev, ListNode* next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}
//将new_node加入到list头部
static inline void ListAddHead(ListNode* new_node, List* list)
{
    ListInsert_(new_node, list, list->next);
}
//将new_node加入到list尾部
static inline void ListAddTail(ListNode* new_node, List* list)
{
    ListInsert_(new_node, list->prev, list);
}

static inline void ListDelete_(ListNode* prev, ListNode* next)
{
    next->prev = prev;
    prev->next = next;
}
//将node从其list中移除
static inline void ListDelete(ListNode* node)
{
    ListDelete_(node->prev, node->next);
}

//遍历双向循环链表，注意：不能删除节点
#define ListForeach(node, list)    \
    for(node =(list)->next; node != (list); node = node->next)

#ifdef _DEBUG
union TestListNode
{
    ListNode node;
};
#endif

}

#endif
