/*******************************************************************************
 *  Repository for C modules.
 *  Copyright (C) 2012 Sandeep Prakash
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ******************************************************************************/

/*******************************************************************************
 * Copyright (c) 2012, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   list_private.h
 *
 * \author sandeepprakash
 *
 * \date   19-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __LIST_PRIVATE_H__
#define __LIST_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LIST_LOG_STR                       "LIST"

#define LIST_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (LIST_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LIST_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (LIST_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LIST_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (LIST_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LIST_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (LIST_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _LIST_NODE_X LIST_NODE_X;

struct _LIST_NODE_X
{
   LIST_NODE_DATA_X x_node_data;

   LIST_NODE_X *px_prev;

   LIST_NODE_X *px_next;
};

typedef struct _LIST_CTXT_X
{
   LIST_INIT_PARAMS_X x_init_params;

   uint32_t ui_node_count;

   LIST_NODE_X *px_head;

   LIST_NODE_X *px_tail;
} LIST_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/
LIST_RET_E list_create_private (
   LIST_CTXT_X **ppx_list_ctxt,
   LIST_INIT_PARAMS_X *px_init_params);

LIST_RET_E list_delete_private (
   LIST_CTXT_X *px_list_ctxt);

LIST_NODE_X *list_alloc_node (
   LIST_NODE_DATA_X *px_node_data);

void list_dealloc_node (
   LIST_NODE_X *px_node);

LIST_RET_E list_node_append_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_NODE_X *px_new_node);

LIST_RET_E list_node_prepend_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_NODE_X *px_new_node);

LIST_RET_E list_front_back_split_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_CTXT_X **ppx_front_list_ctxt,
   LIST_CTXT_X **ppx_back_list_ctxt);

LIST_RET_E list_sorted_merge_private (
   LIST_CTXT_X *px_list1_ctxt,
   LIST_CTXT_X *px_list2_ctxt,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data);

LIST_RET_E list_merge_sort_private (
   LIST_CTXT_X *px_list_ctxt,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data);

#ifdef   __cplusplus
}
#endif

#endif /* __LIST_PRIVATE_H__ */
