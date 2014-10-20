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
 * \file   exp_hashmap.h
 *
 * \author sandeepprakash
 *
 * \date   Mar 17, 2013
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_HASHMAP_H__
#define __EXP_HASHMAP_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/
typedef enum _HM_RET_E
{
   eHM_RET_SUCCESS             = 0x00000000,

   eHM_RET_FAILURE,

   eHM_RET_INVALID_ARGS,

   eHM_RET_INVALID_HANDLE,

   eHM_RET_RESOURCE_FAILURE,

   eHM_RET_HM_EMPTY,

   eHM_RET_HM_FULL,

   eHM_RET_HM_TABLE_ENTRY_NOT_FOUND,

   eHM_RET_HM_NODE_FOUND,

   eHM_RET_HM_NODE_NOT_FOUND,

   eHM_RET_NOT_IMPLEMENTED,

   eHM_RET_CMP_GREATER,

   eHM_RET_CMP_LESSER,

   eHM_RET_CMP_EQUAL,

   eHM_RET_MAX
} HM_RET_E;

typedef enum _HM_KEY_TYPE_E
{
   eHM_KEY_TYPE_INVALID,

   eHM_KEY_TYPE_INT,

   eHM_KEY_TYPE_STRING,

   eHM_KEY_TYPE_MAX
} HM_KEY_TYPE_E;

typedef enum _HM_THREAD_SAFE_FLAGS_BM_E
{
   eHM_THREAD_SAFE_FLAGS_BM_INVALID          = 0x00000000,

   eHM_THREAD_SAFE_FLAGS_BM_MAP_LEVEL        = 0x00000001,

   eHM_THREAD_SAFE_FLAGS_BM_RECORD_LEVEL     = 0x00000002,

   eHM_THREAD_SAFE_FLAGS_BM_MAX
} HM_THREAD_SAFE_FLAGS_BM_E;

typedef enum _HM_LINKED_LIST_FLAGS_BM_E
{
   eHM_LINKED_LIST_FLAGS_BM_INVALID          = 0x00000000,

   eHM_LINKED_LIST_FLAGS_BM_UNSORTED         = 0x00000001,

   eHM_LINKED_LIST_FLAGS_BM_SORTED           = 0x00000002,

   eHM_LINKED_LIST_FLAGS_BM_MAX
} HM_LINKED_LIST_FLAGS_BM_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct HM_CTXT_X       *HM_HDL;

typedef union _HM_KEY_U
{
   uint32_t ui_uint_key;

   uint8_t *puc_str_key;
} HM_KEY_U;

typedef struct _HM_NODE_DATA_X
{
   HM_KEY_TYPE_E e_hm_key_type;

   HM_KEY_U u_hm_key;

   void *p_data;

   uint32_t ui_data_size;
} HM_NODE_DATA_X;

typedef enum _HM_DATA_STRUCT_E
{
   eHM_DATA_STRUCT_INVALID          = 0x00000000,

   eHM_DATA_STRUCT_HM,

   eHM_DATA_STRUCT_LINKED_LIST,

   eHM_DATA_STRUCT_MAX
} HM_DATA_STRUCTURE_E;

typedef enum _HM_FOR_EACH_DIRECTION_E
{
   eHM_FOR_EACH_DIRECTION_INVALID         = 0x00000000,

   eHM_FOR_EACH_DIRECTION_FORWARD,

   eHM_FOR_EACH_DIRECTION_BACKWARD,

   eHM_FOR_EACH_DIRECTION_MAX
} HM_FOR_EACH_DIRECTION_E;

typedef struct _HM_FOR_EACH_PARAMS_X
{
   HM_DATA_STRUCTURE_E  e_data_structure;

   HM_FOR_EACH_DIRECTION_E e_direction;
} HM_FOR_EACH_PARAMS_X;

typedef HM_RET_E (*pfn_hm_for_each_cbk)(
   HM_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

/*
 * - Return
 *    - eHM_RET_CMP_GREATER -  px_node_a_data > px_node_b_data based on some
 *                               application logic.
 *    - eHM_RET_CMP_LESSER  -  px_node_a_data < px_node_b_data based on some
 *                               application logic.
 *    - eHM_RET_CMP_EQUAL   -  px_node_a_data = px_node_b_data based on some
 *                               application logic.
 */
typedef HM_RET_E (*pfn_hm_compare_cbk) (
      HM_NODE_DATA_X *px_node_a_data,
      HM_NODE_DATA_X *px_node_b_data,
      void *p_app_data);

typedef struct _HM_INIT_PARAMS_X
{
   uint32_t ui_hm_table_size;

   HM_KEY_TYPE_E e_hm_key_type;

   bool b_thread_safe;

   uint32_t ui_thread_safe_flags;
   /*!< #_HM_THREAD_SAFE_FLAGS_BM_E */

   bool b_maintain_linked_list;

   uint32_t ui_linked_list_flags;
   /*!< #_HM_LINKED_LIST_FLAGS_BM_E */

   /*!< if _HM_LINKED_LIST_FLAGS_BM_E::eHM_LINKED_LIST_FLAGS_BM_SORTED is set
    *   in  _HM_INIT_PARAMS_X::ui_linked_list_flags. */
   pfn_hm_compare_cbk fn_hm_compare_cbk;

   void *p_compare_app_data;
} HM_INIT_PARAMS_X;

/***************************** FUNCTION PROTOTYPES ****************************/
HM_RET_E hm_create (
   HM_HDL *phl_hm_hdl,
   HM_INIT_PARAMS_X *px_init_params);

HM_RET_E hm_delete (
   HM_HDL hl_hm_hdl);

HM_RET_E hm_add_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_delete_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_delete_all_nodes (
   HM_HDL hl_hm_hdl);

HM_RET_E hm_for_each (
   HM_HDL hl_hm_hdl,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_for_each_v2 (
   HM_HDL hl_hm_hdl,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data);

HM_RET_E hm_search_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_get_total_count (
   HM_HDL hl_hm_hdl,
   uint32_t *pui_total_count);

HM_RET_E hm_linked_list_sort (
   HM_HDL hl_hm_hdl,
   pfn_hm_compare_cbk fn_compare_fn_cbk,
   void *p_app_data);

void hm_print_all_nodes (
   HM_HDL hl_hm_hdl);

HM_RET_E hm_linked_list_peek_at_head (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data);

HM_RET_E hm_linked_list_peek_at_tail (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_HASHMAP_H__ */
