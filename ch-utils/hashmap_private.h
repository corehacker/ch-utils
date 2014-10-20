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
 * Copyright (c) 2013, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   hashmap_private.h
 *
 * \author sandeepprakash
 *
 * \date   Mar 17, 2013
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __HASHMAP_PRIVATE_H__
#define __HASHMAP_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define HM_LOG_STR                       "HMAP"

#define HM_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (HM_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define HM_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (HM_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define HM_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (HM_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define HM_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (HM_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _HM_NODE_X HM_NODE_X;

typedef struct _HM_TABLE_ENTRY_X
{
   LIST_HDL hl_list_hdl;
} HM_TABLE_ENTRY_X;

typedef struct _HM_LINK_LIST_CTXT_X
{
   LIST_HDL hl_list_hdl;

   uint64_t ull_overhead_bytes;
} HM_LINK_LIST_CTXT_X;

typedef struct _HM_CTXT_X
{
   HM_INIT_PARAMS_X x_init_params;

   HM_TABLE_ENTRY_X **ppx_hash_table;

   uint32_t ui_node_count;

   PAL_MUTEX_HDL hl_mutex_hdl;

   HM_LINK_LIST_CTXT_X   *px_link_list_ctxt;
} HM_CTXT_X;

typedef struct _HM_FOR_EACH_METADATA_X
{
   HM_CTXT_X *px_hm_ctxt;

   pfn_hm_for_each_cbk fn_hm_for_each_cbk;

   void *p_hm_for_each_app_data;
} HM_FOR_EACH_METADATA_X;

typedef struct _HM_COMPARE_METADATA_X
{
   HM_CTXT_X *px_hm_ctxt;

   pfn_hm_compare_cbk fn_hm_compare_cbk;

   void *p_hm_compare_app_data;
} HM_COMPARE_METADATA_X;

/***************************** FUNCTION PROTOTYPES ****************************/
HM_RET_E hm_create_resources (
   HM_CTXT_X *px_hm_ctxt);

HM_RET_E hm_delete_resources (
   HM_CTXT_X *px_hm_ctxt);

HM_TABLE_ENTRY_X *hm_get_new_table_entry (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_delete_table_entry_from_hash (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data);

HM_TABLE_ENTRY_X *hm_get_table_entry (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data);

HM_NODE_DATA_X *hm_alloc_node (
   HM_NODE_DATA_X *px_node_data);

void hm_dealloc_node (
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_for_each_hm_forward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_hm_backward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_linked_list_forward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_linked_list_backward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_hm (
   HM_CTXT_X *px_hm_ctxt,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_linked_list (
   HM_CTXT_X *px_hm_ctxt,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_delete_all_nodes_pvt (
   HM_CTXT_X *px_hm_ctxt);

#ifdef   __cplusplus
}
#endif

#endif /* __HASHMAP_PRIVATE_H__ */
