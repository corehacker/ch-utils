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
 * \file   hashmap.c
 *
 * \author sandeepprakash
 *
 * \date   Mar 17, 2013
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_hashmap.h>
#include "hashmap_private.h"
#include "hashmap_private_cbks.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
HM_RET_E hm_create (
   HM_HDL *phl_hm_hdl,
   HM_INIT_PARAMS_X *px_init_params)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_RET_E e_error_pvt = eHM_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;

   if ((NULL == phl_hm_hdl) || (NULL == px_init_params))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((0 == px_init_params->ui_hm_table_size) ||
         (px_init_params->e_hm_key_type <= eHM_KEY_TYPE_INVALID) ||
         (px_init_params->e_hm_key_type >= eHM_KEY_TYPE_MAX))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((eHM_LINKED_LIST_FLAGS_BM_SORTED & px_init_params->ui_linked_list_flags) &&
         (NULL == px_init_params->fn_hm_compare_cbk))
   {
      HM_LOG_MED("Invalid Args. px_init_params->fn_hm_compare_cbk has to be set "
         "if eHM_LINKED_LIST_FLAGS_BM_SORTED is set in "
         "px_init_params->ui_linked_list_flags");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = pal_malloc (sizeof(HM_CTXT_X), NULL);
   if (NULL == px_hm_ctxt)
   {
      HM_LOG_MED("pal_malloc failed");
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   (void) pal_memmove(&(px_hm_ctxt->x_init_params), px_init_params,
      sizeof(px_hm_ctxt->x_init_params));

   e_error = hm_create_resources (px_hm_ctxt);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hm_create_resources failed: %d", e_error);
      e_error_pvt = hm_delete_resources (px_hm_ctxt);
      if (eHM_RET_SUCCESS != e_error_pvt)
      {
         HM_LOG_MED("hm_delete_resources failed: %d", e_error);
      }
   }
   else
   {
      *phl_hm_hdl = (HM_HDL) px_hm_ctxt;
   }
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_delete (
   HM_HDL hl_hm_hdl)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;

   if (NULL == hl_hm_hdl)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   e_error = hm_delete_resources (px_hm_ctxt);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hm_delete_resources failed: %d", e_error);
   }

   pal_free (px_hm_ctxt);
   px_hm_ctxt = NULL;
   e_error = eHM_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_add_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   HM_NODE_DATA_X *px_new_node = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_NODE_DATA_X x_node_data = {NULL};
   bool b_locked = false;
   HM_COMPARE_METADATA_X x_metadata = {NULL};

   if ((NULL == hl_hm_hdl) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   px_hm_table_entry = hm_get_new_table_entry (px_hm_ctxt, px_node_data);
   if (NULL == px_hm_table_entry)
   {
      HM_LOG_MED("hm_get_table_entry failed");
      e_error = eHM_RET_HM_TABLE_ENTRY_NOT_FOUND;
      goto CLEAN_RETURN;
   }

   px_new_node = hm_alloc_node (px_node_data);
   if (NULL == px_new_node)
   {
      HM_LOG_MED("hm_alloc_node failed");
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_node_data.p_data = px_new_node;
   x_node_data.ui_data_size = sizeof(*px_new_node);
   e_list_ret = list_node_append (px_hm_table_entry->hl_list_hdl,
      &x_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      HM_LOG_MED("list_node_append failed: %d", e_list_ret);
      (void) hm_dealloc_node (px_new_node);
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   if ((NULL != px_hm_ctxt->px_link_list_ctxt)
      && (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      if (eHM_LINKED_LIST_FLAGS_BM_UNSORTED
         & px_hm_ctxt->x_init_params.ui_linked_list_flags)
      {
         e_list_ret = list_node_append (
            px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_node_data);
      }
      else if (eHM_LINKED_LIST_FLAGS_BM_SORTED
         & px_hm_ctxt->x_init_params.ui_linked_list_flags)
      {
         x_metadata.px_hm_ctxt = px_hm_ctxt;
         x_metadata.fn_hm_compare_cbk =
            px_hm_ctxt->x_init_params.fn_hm_compare_cbk;
         x_metadata.p_hm_compare_app_data =
            px_hm_ctxt->x_init_params.p_compare_app_data;
         e_list_ret = list_node_insert_sorted (
            px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_node_data,
            hm_list_compare_fn_cbk, &x_metadata);
      }
      else
      {
         HM_LOG_MED("Invalid Args");
         (void) hm_dealloc_node (px_new_node);
         e_error = eHM_RET_INVALID_ARGS;
      }

      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         HM_LOG_MED("list_node_append/list_node_insert_sorted failed: %d",
            e_list_ret);

         (void) list_node_delete_at_tail (px_hm_table_entry->hl_list_hdl,
            &x_node_data);

         (void) hm_dealloc_node (px_new_node);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
      else
      {
         px_hm_ctxt->ui_node_count++;
         e_error = eHM_RET_SUCCESS;
      }
   }
   else
   {
      px_hm_ctxt->ui_node_count++;
      e_error = eHM_RET_SUCCESS;
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_delete_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_RET_E e_error_priv = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   HM_CTXT_X *px_hm_ctxt = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   uint32_t ui_list_len = 0;
   LIST_NODE_DATA_X x_list_search_node_data = {NULL};
   LIST_NODE_DATA_X x_list_found_node_data = {NULL};
   HM_NODE_DATA_X *px_found_node = NULL;
   bool b_locked = false;

   if ((NULL == hl_hm_hdl) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   px_hm_table_entry = hm_get_table_entry (px_hm_ctxt, px_node_data);
   if (NULL == px_hm_table_entry)
   {
      HM_LOG_FULL("hm_get_table_entry failed");
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }
   if (NULL == px_hm_table_entry->hl_list_hdl)
   {
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }
   e_list_ret = list_get_len (px_hm_table_entry->hl_list_hdl,
      &ui_list_len);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (0 == ui_list_len))
   {
      HM_LOG_FULL("list_get_len failed: %d, %d", e_list_ret, ui_list_len);
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }

   x_list_search_node_data.p_data = px_node_data;
   x_list_search_node_data.ui_data_size = sizeof(*px_node_data);

   if ((NULL != px_hm_ctxt->px_link_list_ctxt)
         && (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      e_list_ret = list_node_delete_after_search (
         px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_list_search_node_data,
         hm_list_search_cbk, px_hm_ctxt, &x_list_found_node_data);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         HM_LOG_HIGH("list_node_delete_after_search failed: %d", e_list_ret);
         e_error = eHM_RET_HM_NODE_NOT_FOUND;
      }
   }

   e_list_ret = list_node_delete_after_search (px_hm_table_entry->hl_list_hdl,
      &x_list_search_node_data, hm_list_search_cbk, px_hm_ctxt,
      &x_list_found_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      HM_LOG_HIGH("list_node_delete_after_search failed: %d", e_list_ret);
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
   }
   else
   {
      if ((x_list_found_node_data.ui_data_size == sizeof(*px_node_data)) &&
            (NULL != x_list_found_node_data.p_data))
      {
         px_found_node = (HM_NODE_DATA_X *) x_list_found_node_data.p_data;
         px_node_data->p_data = px_found_node->p_data;
         px_node_data->ui_data_size = px_found_node->ui_data_size;

         HM_LOG_MED("Deleted node data: %p", px_node_data->p_data);

         (void) hm_dealloc_node(px_found_node);
         px_found_node = NULL;

         e_list_ret = list_get_len (px_hm_table_entry->hl_list_hdl,
            &ui_list_len);
         if ((eLIST_RET_SUCCESS == e_list_ret) && (0 == ui_list_len))
         {
            e_error_priv = hm_delete_table_entry_from_hash (px_hm_ctxt, px_node_data);
            if (eHM_RET_SUCCESS != e_error_priv)
            {
               HM_LOG_MED("hm_delete_table_entry failed: %d", e_error_priv);
            }
            px_hm_table_entry = NULL;
         }
         px_hm_ctxt->ui_node_count--;
         e_error = eHM_RET_SUCCESS;
      }
      else
      {
         HM_LOG_HIGH("list_node_delete_after_search failed: %d", e_list_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_delete_all_nodes (
   HM_HDL hl_hm_hdl)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;

   if (NULL == hl_hm_hdl)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;
   e_error = hm_delete_all_nodes_pvt (px_hm_ctxt);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hm_delete_all_nodes_pvt failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_for_each (
   HM_HDL hl_hm_hdl,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;

   if ((NULL == hl_hm_hdl) || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   e_error = hm_for_each_hm_forward (px_hm_ctxt, fn_for_each_cbk, p_app_data);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hm_for_each_forward failed: %d", e_error);
   }

CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_for_each_v2 (
   HM_HDL hl_hm_hdl,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;

   if ((NULL == hl_hm_hdl) || (NULL == px_for_each_param)
      || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;
   switch (px_for_each_param->e_data_structure)
   {
      case eHM_DATA_STRUCT_HM:
      {
         e_error = hm_for_each_hm (px_hm_ctxt, px_for_each_param,
            fn_for_each_cbk, p_app_data);
         break;
      }
      case eHM_DATA_STRUCT_LINKED_LIST:
      {
         e_error = hm_for_each_linked_list (px_hm_ctxt, px_for_each_param,
            fn_for_each_cbk, p_app_data);
         break;
      }
      default:
      {
         HM_LOG_MED("Invalid Args");
         break;
      }
   }
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_search_node (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   uint32_t ui_list_len = 0;
   LIST_NODE_DATA_X x_list_search_node_data = {NULL};
   LIST_NODE_DATA_X x_list_found_node_data = {NULL};
   HM_NODE_DATA_X *px_found_node = NULL;
   bool b_locked = false;

   if ((NULL == hl_hm_hdl) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   px_hm_table_entry = hm_get_table_entry (px_hm_ctxt, px_node_data);
   if (NULL == px_hm_table_entry)
   {
      HM_LOG_FULL("hm_get_table_entry failed");
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }
   if (NULL == px_hm_table_entry->hl_list_hdl)
   {
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }
   e_list_ret = list_get_len (px_hm_table_entry->hl_list_hdl,
      &ui_list_len);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (0 == ui_list_len))
   {
      HM_LOG_FULL("list_get_len failed: %d, %d", e_list_ret, ui_list_len);
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
      goto CLEAN_RETURN;
   }

   x_list_search_node_data.p_data = px_node_data;
   x_list_search_node_data.ui_data_size = sizeof(*px_node_data);

   e_list_ret = list_search_node (px_hm_table_entry->hl_list_hdl,
      &x_list_search_node_data, hm_list_search_cbk, px_hm_ctxt,
      &x_list_found_node_data);
   if (eLIST_RET_LIST_NODE_FOUND != e_list_ret)
   {
      e_error = eHM_RET_HM_NODE_NOT_FOUND;
   }
   else
   {
      px_found_node = (HM_NODE_DATA_X *) x_list_found_node_data.p_data;
      px_node_data->p_data = px_found_node->p_data;
      px_node_data->ui_data_size = px_found_node->ui_data_size;
      e_error = eHM_RET_HM_NODE_FOUND;
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_get_total_count (
   HM_HDL hl_hm_hdl,
   uint32_t *pui_total_count)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   bool b_locked = false;

   if ((NULL == hl_hm_hdl) || (NULL == pui_total_count))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   *pui_total_count = px_hm_ctxt->ui_node_count;
   e_error = eHM_RET_SUCCESS;
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_linked_list_sort (
   HM_HDL hl_hm_hdl,
   pfn_hm_compare_cbk fn_compare_fn_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   bool b_locked = false;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_SORT_PARAMS_X x_sort_params = {eLIST_SORT_ALGORITHM_INVALID};
   HM_COMPARE_METADATA_X x_metadata = {NULL};

   if ((NULL == hl_hm_hdl) || (NULL == fn_compare_fn_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   if ((NULL != px_hm_ctxt->px_link_list_ctxt)
      && (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      x_metadata.px_hm_ctxt = px_hm_ctxt;
      x_metadata.fn_hm_compare_cbk = fn_compare_fn_cbk;
      x_metadata.p_hm_compare_app_data = p_app_data;

      x_sort_params.e_sort_algorithm = eLIST_SORT_ALGORITHM_MERGE_SORT;
      x_sort_params.fn_compare_fn_cbk = hm_list_compare_fn_cbk;
      x_sort_params.p_app_data = &x_metadata;
      e_list_ret = list_sort (px_hm_ctxt->px_link_list_ctxt->hl_list_hdl,
         &x_sort_params);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         HM_LOG_MED("list_sort failed: %d", e_list_ret);
         if (eLIST_RET_LIST_EMPTY == e_list_ret)
         {
            HM_LOG_MED("List empty");
            e_error = eHM_RET_HM_EMPTY;
         }
         else
         {
            e_error = eHM_RET_RESOURCE_FAILURE;
         }
      }
      else
      {
         e_error = eHM_RET_SUCCESS;
      }
   }
   else
   {
      HM_LOG_MED("Linked list has not been enabled by application");
      e_error = eHM_RET_RESOURCE_FAILURE;
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

void hm_print_all_nodes (
   HM_HDL hl_hm_hdl)
{
   HM_CTXT_X *px_hm_ctxt = NULL;
   uint32_t ui_i = 0;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   uint32_t ui_list_len = 0;
   bool b_locked = false;

   if (NULL != hl_hm_hdl)
   {
      px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

      if (NULL != px_hm_ctxt->hl_mutex_hdl)
      {
         e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
         if (ePAL_RET_SUCCESS != e_pal_ret)
         {
            HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         }
         b_locked = true;
      }

      printf (
         "\n\n*******************HASH MAP CONTENTS STARTS******************\n");

      for (ui_i = 0; ui_i < px_hm_ctxt->x_init_params.ui_hm_table_size; ui_i++)
      {
         px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_i];
         if (NULL == px_hm_table_entry)
         {
            continue;
         }
         if (NULL == px_hm_table_entry->hl_list_hdl)
         {
            continue;
         }
         e_list_ret = list_get_len (px_hm_table_entry->hl_list_hdl,
            &ui_list_len);
         if ((eLIST_RET_SUCCESS != e_list_ret) || (0 == ui_list_len))
         {
            continue;
         }

         printf ("@ %d\n", ui_i);
         e_list_ret = list_for_all_nodes (px_hm_table_entry->hl_list_hdl,
            hm_print_nodes_for_all_cbk, px_hm_table_entry);
      }

      printf (
         "\n*******************HASH MAP CONTENTS ENDS******************\n");

      if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
      {
         e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
         if (ePAL_RET_SUCCESS != e_pal_ret)
         {
            HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         }
      }
   }
}

HM_RET_E hm_linked_list_peek_at_head (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   bool b_locked = false;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_NODE_DATA_X x_list_node_data = {NULL};

   if ((NULL == hl_hm_hdl) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   if ((NULL == px_hm_ctxt->px_link_list_ctxt)
      || (NULL == px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      HM_LOG_MED("Linked list has not been enabled by application");
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_list_ret = list_node_peek_at_head (
      px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_list_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      HM_LOG_MED("list_node_peek_at_head failed: %d", e_list_ret);
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         HM_LOG_MED("List empty");
         e_error = eHM_RET_HM_EMPTY;
      }
      else
      {
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      (void) pal_memmove (px_node_data, x_list_node_data.p_data,
         sizeof(*px_node_data));
      e_error = eHM_RET_SUCCESS;
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_linked_list_peek_at_tail (
   HM_HDL hl_hm_hdl,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   HM_CTXT_X *px_hm_ctxt = NULL;
   bool b_locked = false;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_NODE_DATA_X x_list_node_data = {NULL};

   if ((NULL == hl_hm_hdl) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_ctxt = (HM_CTXT_X *) hl_hm_hdl;

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   if ((NULL == px_hm_ctxt->px_link_list_ctxt)
      || (NULL == px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      HM_LOG_MED("Linked list has not been enabled by application");
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_list_ret = list_node_peek_at_tail (
      px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_list_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      HM_LOG_MED("list_node_peek_at_tail failed: %d", e_list_ret);
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         HM_LOG_MED("List empty");
         e_error = eHM_RET_HM_EMPTY;
      }
      else
      {
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      (void) pal_memmove (px_node_data, x_list_node_data.p_data,
         sizeof(*px_node_data));
      e_error = eHM_RET_SUCCESS;
   }

CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}
