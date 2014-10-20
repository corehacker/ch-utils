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
 * \file   exp_list.h
 *
 * \author sandeepprakash
 *
 * \date   19-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_LIST_H__
#define __EXP_LIST_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LIST_SIZE_UNLIMITED               (PAL_UINT32_MAX_VALUE)

/******************************** ENUMERATIONS ********************************/
typedef enum _LIST_RET_E
{
   eLIST_RET_SUCCESS             = 0x00000000,

   eLIST_RET_FAILURE,

   eLIST_RET_INVALID_ARGS,

   eLIST_RET_INVALID_HANDLE,

   eLIST_RET_RESOURCE_FAILURE,

   eLIST_RET_LIST_EMPTY,

   eLIST_RET_LIST_FULL,

   eLIST_RET_LIST_NODE_FOUND,

   eLIST_RET_LIST_NODE_NOT_FOUND,

   eLIST_RET_NOT_IMPLEMENTED,

   eLIST_RET_CMP_GREATER,

   eLIST_RET_CMP_LESSER,

   eLIST_RET_CMP_EQUAL,

   eLIST_RET_MAX
} LIST_RET_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct LIST_CTXT_X       *LIST_HDL;

typedef struct _LIST_INIT_PARAMS_X
{
   uint32_t ui_list_max_elements;
} LIST_INIT_PARAMS_X;

typedef struct _LIST_NODE_DATA_X
{
   void *p_data;

   uint32_t ui_data_size;
} LIST_NODE_DATA_X;

typedef LIST_RET_E (*pfn_list_for_all_cbk) (
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

typedef LIST_RET_E (*pfn_list_search_cbk) (
   LIST_NODE_DATA_X *px_search_node_data,
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

/*
 * - Return
 *    - eLIST_RET_CMP_GREATER -  px_node_a_data > px_node_b_data based on some
 *                               application logic.
 *    - eLIST_RET_CMP_LESSER  -  px_node_a_data < px_node_b_data based on some
 *                               application logic.
 *    - eLIST_RET_CMP_EQUAL   -  px_node_a_data = px_node_b_data based on some
 *                               application logic.
 */
typedef LIST_RET_E (*pfn_list_compare_fn_cbk) (
      LIST_NODE_DATA_X *px_node_a_data,
      LIST_NODE_DATA_X *px_node_b_data,
      void *p_app_data);

typedef enum _LIST_SORT_ALGORITHM_E
{
   eLIST_SORT_ALGORITHM_INVALID           = 0x00000000,

   eLIST_SORT_ALGORITHM_MERGE_SORT,

   eLIST_SORT_ALGORITHM_MAX
} LIST_SORT_ALGORITHM_E;

typedef struct _LIST_SORT_PARAMS_X
{
   LIST_SORT_ALGORITHM_E e_sort_algorithm;

   pfn_list_compare_fn_cbk fn_compare_fn_cbk;

   void *p_app_data;
} LIST_SORT_PARAMS_X;

/***************************** FUNCTION PROTOTYPES ****************************/
LIST_RET_E list_create (
   LIST_HDL *phl_list_hdl,
   LIST_INIT_PARAMS_X *px_init_params);

LIST_RET_E list_delete (
   LIST_HDL hl_list_hdl);

LIST_RET_E list_node_append (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

LIST_RET_E list_node_insert_sorted (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data);

LIST_RET_E list_node_prepend (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

LIST_RET_E list_node_delete_at_head (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

LIST_RET_E list_node_delete_at_tail (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

LIST_RET_E list_for_all_nodes (
   LIST_HDL hl_list_hdl,
   pfn_list_for_all_cbk fn_for_all_cbk,
   void *p_app_data);

LIST_RET_E list_search_node (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_search_node_data,
   pfn_list_search_cbk fn_search_cbk,
   void *p_app_data,
   LIST_NODE_DATA_X *px_found_node_data);

LIST_RET_E list_node_delete_after_search (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_search_node_data,
   pfn_list_search_cbk fn_search_cbk,
   void *p_app_data,
   LIST_NODE_DATA_X *px_node_data);

void list_print_all_nodes (
   LIST_HDL hl_list_hdl);

LIST_RET_E list_get_len (
   LIST_HDL hl_list_hdl,
   uint32_t *pui_list_len);

LIST_RET_E list_front_back_split (
   LIST_HDL hl_list_hdl,
   LIST_HDL *phl_front_list_hdl,
   LIST_HDL *phl_back_list_hdl);

LIST_RET_E list_sorted_merge (
   LIST_HDL hl_list1_hdl,
   LIST_HDL hl_list2_hdl,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data);

LIST_RET_E list_sort (
   LIST_HDL hl_list_hdl,
   LIST_SORT_PARAMS_X *px_sort_params);

LIST_RET_E list_node_peek_at_head (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

LIST_RET_E list_node_peek_at_tail (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_LIST_H__ */
