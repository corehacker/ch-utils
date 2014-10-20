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
 * \file   list.c
 *
 * \author sandeepprakash
 *
 * \date   19-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include "list_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
LIST_RET_E list_create (
   LIST_HDL *phl_list_hdl,
   LIST_INIT_PARAMS_X *px_init_params)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;

   if ((NULL == phl_list_hdl) || (NULL == px_init_params))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *phl_list_hdl = NULL;

   e_error = list_create_private (&px_list_ctxt, px_init_params);
   if ((eLIST_RET_SUCCESS != e_error) || (NULL == px_list_ctxt))
   {
      LIST_LOG_MED("list_create_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }
   else
   {
      *phl_list_hdl = (LIST_HDL) px_list_ctxt;
      LIST_LOG_HIGH("List of size %d create success",
         px_init_params->ui_list_max_elements);
   }
CLEAN_RETURN:
   if (eLIST_RET_SUCCESS != e_error)
   {
      (void) list_delete_private (px_list_ctxt);
   }
   return e_error;
}

LIST_RET_E list_delete (
   LIST_HDL hl_list_hdl)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;

   if (NULL == hl_list_hdl)
   {
      LIST_LOG_MED("Invalid Handle");
      e_error = eLIST_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   (void) list_delete_private (px_list_ctxt);
   LIST_LOG_HIGH("List delete success");
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_append (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_new_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if (px_list_ctxt->x_init_params.ui_list_max_elements
      == px_list_ctxt->ui_node_count)
   {
      LIST_LOG_MED("List Full: %d, Max allowed elements: %d", 
            px_list_ctxt->ui_node_count,
            px_list_ctxt->x_init_params.ui_list_max_elements);
      e_error = eLIST_RET_LIST_FULL;
      goto CLEAN_RETURN;
   }

   px_new_node = list_alloc_node(px_node_data);
   if (NULL == px_new_node)
   {
      LIST_LOG_MED("list_alloc_node failed: %p", px_new_node);
      e_error = eLIST_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_error = list_node_append_private (px_list_ctxt, px_new_node);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_node_append_private failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_insert_sorted (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_new_node = NULL;
   LIST_NODE_X *px_curr_node = NULL;
   LIST_NODE_X *px_prev_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data)
      || (NULL == fn_compare_fn_cbk))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if (px_list_ctxt->x_init_params.ui_list_max_elements
      == px_list_ctxt->ui_node_count)
   {
      LIST_LOG_MED("List Full: %d, Max allowed elements: %d",
            px_list_ctxt->ui_node_count,
            px_list_ctxt->x_init_params.ui_list_max_elements);
      e_error = eLIST_RET_LIST_FULL;
      goto CLEAN_RETURN;
   }

   px_new_node = list_alloc_node(px_node_data);
   if (NULL == px_new_node)
   {
      LIST_LOG_MED("list_alloc_node failed: %p", px_new_node);
      e_error = eLIST_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   /*
    * Initial condition.
    */
   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      px_list_ctxt->px_head = px_new_node;
      px_list_ctxt->px_tail = px_new_node;
      px_list_ctxt->ui_node_count += 1;
      LIST_LOG_HIGH("Insert Sorted: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_new_node,
         px_new_node->x_node_data.p_data,
         px_new_node->x_node_data.ui_data_size);
      e_error = eLIST_RET_SUCCESS;
      goto CLEAN_RETURN;
   }

   px_curr_node = px_list_ctxt->px_head;
   px_prev_node = NULL;
   while (NULL != px_curr_node)
   {
      e_error = fn_compare_fn_cbk (px_node_data,
         &(px_curr_node->x_node_data), p_app_data);
      if (eLIST_RET_CMP_LESSER == e_error)
      {
         break;
      }

      px_prev_node = px_curr_node;
      px_curr_node = px_curr_node->px_next;
   }

   if (eLIST_RET_CMP_LESSER == e_error)
   {
      if (NULL == px_prev_node)
      {
         /*
          * Insert the node at the found position in the list.
          */
         px_new_node->px_next = px_list_ctxt->px_head;
         px_list_ctxt->px_head->px_prev = px_new_node;
         px_list_ctxt->px_head = px_new_node;
         px_list_ctxt->ui_node_count += 1;
         LIST_LOG_HIGH("Insert Sorted: Count: %d, Node: %p, Data: %p, Size: %d",
            px_list_ctxt->ui_node_count, px_new_node,
            px_new_node->x_node_data.p_data,
            px_new_node->x_node_data.ui_data_size);
         e_error = eLIST_RET_SUCCESS;
      }
      else
      {
         px_new_node->px_next = px_curr_node;
         px_new_node->px_prev = px_prev_node;
         px_curr_node->px_prev = px_new_node;
         px_prev_node->px_next = px_new_node;
         LIST_LOG_HIGH("Insert Sorted: Count: %d, Node: %p, Data: %p, Size: %d",
            px_list_ctxt->ui_node_count, px_new_node,
            px_new_node->x_node_data.p_data,
            px_new_node->x_node_data.ui_data_size);
         e_error = eLIST_RET_SUCCESS;
      }
   }
   else
   {
      /*
       * Append the node at the tail of the list.
       */
      e_error = list_node_append_private (px_list_ctxt, px_new_node);
      if (eLIST_RET_SUCCESS != e_error)
      {
         LIST_LOG_MED("list_node_append_private failed: %d", e_error);
      }
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_prepend (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_new_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if (px_list_ctxt->x_init_params.ui_list_max_elements
      == px_list_ctxt->ui_node_count)
   {
      LIST_LOG_MED("List Full: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_FULL;
      goto CLEAN_RETURN;
   }

   px_new_node = list_alloc_node(px_node_data);
   if (NULL == px_new_node)
   {
      LIST_LOG_MED("list_alloc_node failed: %p", px_new_node);
      e_error = eLIST_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_error = list_node_prepend_private (px_list_ctxt, px_new_node);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_node_append_private failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_for_all_nodes (
   LIST_HDL hl_list_hdl,
   pfn_list_for_all_cbk fn_for_all_cbk,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == fn_for_all_cbk))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_HIGH("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }
   px_curr_node = px_list_ctxt->px_head;

   while (NULL != px_curr_node)
   {
      e_error = fn_for_all_cbk (&(px_curr_node->x_node_data), p_app_data);
      if (eLIST_RET_SUCCESS != e_error)
      {
         break;
      }
      px_curr_node = px_curr_node->px_next;
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_search_node (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_search_node_data,
   pfn_list_search_cbk fn_search_cbk,
   void *p_app_data,
   LIST_NODE_DATA_X *px_found_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_search_node_data)
         || (NULL == fn_search_cbk))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }
   px_curr_node = px_list_ctxt->px_head;

   while (NULL != px_curr_node)
   {
      e_error = fn_search_cbk (px_search_node_data,
         &(px_curr_node->x_node_data), p_app_data);
      if (eLIST_RET_LIST_NODE_FOUND == e_error)
      {
         if (NULL != px_found_node_data)
         {
            (void) pal_memmove(px_found_node_data, &(px_curr_node->x_node_data),
               sizeof(*px_found_node_data));
         }
         break;
      }
      px_curr_node = px_curr_node->px_next;
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_delete_at_head (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_head = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;
   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_head = px_list_ctxt->px_head;
   px_list_ctxt->px_head = px_head->px_next;
   if (NULL == px_list_ctxt->px_head)
   {
      px_list_ctxt->px_tail = NULL;
   }
   else
   {
      px_list_ctxt->px_head->px_prev = NULL;
   }
   px_list_ctxt->ui_node_count -= 1;

   LIST_LOG_HIGH("Delete Head: Count: %d, Node: %p, Data: %p, Size: %d",
      px_list_ctxt->ui_node_count, px_head,
      px_head->x_node_data.p_data,
      px_head->x_node_data.ui_data_size);

   (void) pal_memmove(px_node_data, &(px_head->x_node_data),
      sizeof(*px_node_data));
   list_dealloc_node (px_head);
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_delete_at_tail (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node = NULL;
   LIST_NODE_X *px_tail = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;
   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_curr_node = px_list_ctxt->px_head;
   px_tail = px_list_ctxt->px_tail;
   while (NULL != px_curr_node)
   {
      if (px_curr_node->px_next == px_tail)
      {
         break;
      }
      px_curr_node = px_curr_node->px_next;
   }

   if (NULL != px_curr_node)
   {
      px_curr_node->px_next = NULL;
   }
   px_list_ctxt->px_tail = px_curr_node;

   if (NULL == px_list_ctxt->px_tail)
   {
      px_list_ctxt->px_head = NULL;
   }
   else
   {
      px_list_ctxt->px_tail->px_next = NULL;
   }
   px_list_ctxt->ui_node_count -= 1;

   LIST_LOG_HIGH("Delete Head: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_tail,
         px_tail->x_node_data.p_data,
         px_tail->x_node_data.ui_data_size);

   (void) pal_memmove(px_node_data, &(px_tail->x_node_data),
      sizeof(*px_node_data));
   list_dealloc_node (px_tail);
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_delete_after_search (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_search_node_data,
   pfn_list_search_cbk fn_search_cbk,
   void *p_app_data,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_search_node_data)
         || (NULL == fn_search_cbk))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_curr_node = px_list_ctxt->px_head;

   while (NULL != px_curr_node)
   {
      e_error = fn_search_cbk (px_search_node_data,
         &(px_curr_node->x_node_data), p_app_data);
      if (eLIST_RET_LIST_NODE_FOUND == e_error)
      {
         break;
      }
      px_curr_node = px_curr_node->px_next;
   }

   if (NULL != px_curr_node)
   {
      if (px_curr_node == px_list_ctxt->px_head)
      {
         e_error = list_node_delete_at_head ((LIST_HDL) px_list_ctxt,
            px_node_data);
         if (eLIST_RET_SUCCESS != e_error)
         {
            LIST_LOG_MED("list_node_delete_at_head failed: %d", e_error);
         }
      }
      else if (px_curr_node == px_list_ctxt->px_tail)
      {
         e_error = list_node_delete_at_tail ((LIST_HDL) px_list_ctxt,
            px_node_data);
         if (eLIST_RET_SUCCESS != e_error)
         {
            LIST_LOG_MED("list_node_delete_at_tail failed: %d", e_error);
         }
      }
      else
      {
         LIST_LOG_HIGH("Delete Item: Count: %d, Node: %p, Data: %p, Size: %d",
            px_list_ctxt->ui_node_count, px_curr_node,
            px_curr_node->x_node_data.p_data,
            px_curr_node->x_node_data.ui_data_size);
         px_curr_node->px_next->px_prev = px_curr_node->px_prev;
         px_curr_node->px_prev->px_next = px_curr_node->px_next;
         px_list_ctxt->ui_node_count -= 1;
         (void) pal_memmove (px_node_data, &(px_curr_node->x_node_data),
            sizeof(*px_node_data));
         list_dealloc_node (px_curr_node);
         e_error = eLIST_RET_SUCCESS;
      }
   }
   else
   {
      e_error = eLIST_RET_LIST_NODE_NOT_FOUND;
   }
CLEAN_RETURN:
   return e_error;
}

void list_print_all_nodes (
   LIST_HDL hl_list_hdl)
{
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node = NULL;

   if (NULL != hl_list_hdl)
   {
      px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;
      px_curr_node = px_list_ctxt->px_head;

      printf ("\nList Contents:\n");
      while (NULL != px_curr_node)
      {
         printf ("[%p,(%p),%p] -> ", px_curr_node->px_prev, px_curr_node,
            px_curr_node->px_next);
         px_curr_node = px_curr_node->px_next;
      }
      printf ("\n");
   }
}

LIST_RET_E list_get_len (
   LIST_HDL hl_list_hdl,
   uint32_t *pui_list_len)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;

   if ((NULL == hl_list_hdl) || (NULL == pui_list_len))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *pui_list_len = 0;
   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_FULL("List Empty: %d", px_list_ctxt->ui_node_count);
   }
   else
   {
      LIST_LOG_FULL("List Len: %d", px_list_ctxt->ui_node_count);
      *pui_list_len = px_list_ctxt->ui_node_count;
   }
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_front_back_split (
   LIST_HDL hl_list_hdl,
   LIST_HDL *phl_front_list_hdl,
   LIST_HDL *phl_back_list_hdl)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_CTXT_X *px_front_list_ctxt = NULL;
   LIST_CTXT_X *px_back_list_ctxt = NULL;

   if ((NULL == hl_list_hdl) || (NULL == phl_front_list_hdl)
      || (NULL == phl_back_list_hdl))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   e_error = list_front_back_split_private (px_list_ctxt, &px_front_list_ctxt,
      &px_back_list_ctxt);
   if ((eLIST_RET_SUCCESS != e_error) || (NULL == px_front_list_ctxt)
      || (NULL == px_back_list_ctxt))
   {
      LIST_LOG_MED("list_front_back_split_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }
   else
   {
      *phl_front_list_hdl = (LIST_HDL) px_front_list_ctxt;
      *phl_back_list_hdl = (LIST_HDL) px_back_list_ctxt;
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_sorted_merge (
   LIST_HDL hl_list1_hdl,
   LIST_HDL hl_list2_hdl,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list1_ctxt = NULL;
   LIST_CTXT_X *px_list2_ctxt = NULL;

   if ((NULL == hl_list1_hdl) || (NULL == hl_list2_hdl)
      || (NULL == fn_compare_fn_cbk))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list1_ctxt = (LIST_CTXT_X *) hl_list1_hdl;
   px_list2_ctxt = (LIST_CTXT_X *) hl_list2_hdl;

   e_error = list_sorted_merge_private (px_list1_ctxt, px_list2_ctxt,
      fn_compare_fn_cbk, p_app_data);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_sorted_merge_private failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_sort (
   LIST_HDL hl_list_hdl,
   LIST_SORT_PARAMS_X *px_sort_params)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_sort_params))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL == px_sort_params->fn_compare_fn_cbk)
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;

   switch (px_sort_params->e_sort_algorithm)
   {
      case eLIST_SORT_ALGORITHM_MERGE_SORT:
      {
         if (0 == px_list_ctxt->ui_node_count)
         {
            e_error = eLIST_RET_LIST_EMPTY;
         }
         else
         {
            e_error = list_merge_sort_private (px_list_ctxt,
               px_sort_params->fn_compare_fn_cbk, px_sort_params->p_app_data);
            if (eLIST_RET_SUCCESS != e_error)
            {
               LIST_LOG_MED("list_sorted_merge_private failed: %d", e_error);
            }
         }
         break;
      }
      default:
      {
         LIST_LOG_MED("Invalid Args");
         break;
      }
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_peek_at_head (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_head = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;
   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_head = px_list_ctxt->px_head;

   LIST_LOG_HIGH("Peek Head: Count: %d, Node: %p, Data: %p, Size: %d",
      px_list_ctxt->ui_node_count, px_head,
      px_head->x_node_data.p_data,
      px_head->x_node_data.ui_data_size);

   (void) pal_memmove(px_node_data, &(px_head->x_node_data),
      sizeof(*px_node_data));
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_peek_at_tail (
   LIST_HDL hl_list_hdl,
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;
   LIST_NODE_X *px_tail = NULL;

   if ((NULL == hl_list_hdl) || (NULL == px_node_data))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_list_ctxt = (LIST_CTXT_X *) hl_list_hdl;
   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_tail = px_list_ctxt->px_tail;

   LIST_LOG_HIGH("Peek Head: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_tail,
         px_tail->x_node_data.p_data,
         px_tail->x_node_data.ui_data_size);

   (void) pal_memmove(px_node_data, &(px_tail->x_node_data),
      sizeof(*px_node_data));
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}
