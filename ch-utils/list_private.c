/*******************************************************************************
 *  Repository for C modules.
 *  Copyright (C) 2014 Sandeep Prakash
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
 * Copyright (c) 2014, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   list_private.c
 *
 * \author sandeepprakash
 *
 * \date   Feb 26, 2014
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
LIST_RET_E list_create_private (
   LIST_CTXT_X **ppx_list_ctxt,
   LIST_INIT_PARAMS_X *px_init_params)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_list_ctxt = NULL;

   if ((NULL == ppx_list_ctxt) || (NULL == px_init_params))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *ppx_list_ctxt = NULL;

   px_list_ctxt = pal_malloc (sizeof(LIST_CTXT_X), NULL);
   if (NULL == px_list_ctxt)
   {
      LIST_LOG_MED("pal_malloc failed: %p", px_list_ctxt);
      e_error = eLIST_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   (void) pal_memmove(&(px_list_ctxt->x_init_params), px_init_params,
      sizeof(px_list_ctxt->x_init_params));
   px_list_ctxt->px_head = NULL;
   px_list_ctxt->px_tail = NULL;
   px_list_ctxt->ui_node_count = 0;
   *ppx_list_ctxt = px_list_ctxt;
   LIST_LOG_HIGH("List of size %d create success",
      px_init_params->ui_list_max_elements);
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   if (eLIST_RET_SUCCESS != e_error)
   {
      if (NULL != px_list_ctxt)
      {
         pal_free (px_list_ctxt);
         px_list_ctxt = NULL;
      }
   }
   return e_error;
}

LIST_RET_E list_delete_private (
   LIST_CTXT_X *px_list_ctxt)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;

   if (NULL == px_list_ctxt)
   {
      LIST_LOG_MED("Invalid Handle");
      e_error = eLIST_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   pal_free(px_list_ctxt);
   px_list_ctxt = NULL;
   LIST_LOG_HIGH("List delete success");
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_NODE_X *list_alloc_node (
   LIST_NODE_DATA_X *px_node_data)
{
   LIST_NODE_X *px_node = NULL;

   if (NULL != px_node_data)
   {
      px_node = pal_malloc(sizeof(LIST_NODE_X), NULL);
      if (NULL != px_node)
      {
         (void) pal_memmove(&(px_node->x_node_data), px_node_data,
            sizeof(px_node->x_node_data));
      }
      else
      {
         LIST_LOG_MED("pal_malloc failed: %p", px_node);
      }
   }
   else
   {
      LIST_LOG_MED("Invalid Args");
   }
   return px_node;
}

void list_dealloc_node (
   LIST_NODE_X *px_node)
{
   if (NULL != px_node)
   {
      pal_free(px_node);
      px_node = NULL;
   }
}

LIST_RET_E list_node_append_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_NODE_X *px_new_node)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_NODE_X *px_tail = NULL;

   if ((NULL == px_list_ctxt) || (NULL == px_new_node))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
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
      LIST_LOG_HIGH("Append: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_new_node,
         px_new_node->x_node_data.p_data,
         px_new_node->x_node_data.ui_data_size);
   }
   else
   {
      /*
       * Append the node at the tail of the list.
       */
      px_tail = px_list_ctxt->px_tail;
      px_tail->px_next = px_new_node;
      px_new_node->px_prev = px_tail;
      px_list_ctxt->px_tail = px_tail->px_next;
      px_list_ctxt->ui_node_count += 1;
      LIST_LOG_HIGH("Append: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_new_node,
         px_new_node->x_node_data.p_data,
         px_new_node->x_node_data.ui_data_size);
   }
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_node_prepend_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_NODE_X *px_new_node)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;

   if ((NULL == px_list_ctxt) || (NULL == px_new_node))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
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
      LIST_LOG_HIGH("Prepend: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_new_node,
         px_new_node->x_node_data.p_data,
         px_new_node->x_node_data.ui_data_size);
   }
   else
   {
      /*
       * Append the node at the head of the list.
       */
      px_new_node->px_next = px_list_ctxt->px_head;
      px_list_ctxt->px_head->px_prev = px_new_node;
      px_list_ctxt->px_head = px_new_node;
      px_list_ctxt->ui_node_count += 1;
      LIST_LOG_HIGH("Prepend: Count: %d, Node: %p, Data: %p, Size: %d",
         px_list_ctxt->ui_node_count, px_new_node,
         px_new_node->x_node_data.p_data,
         px_new_node->x_node_data.ui_data_size);
   }
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_front_back_split_private (
   LIST_CTXT_X *px_list_ctxt,
   LIST_CTXT_X **ppx_front_list_ctxt,
   LIST_CTXT_X **ppx_back_list_ctxt)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_front_list_ctxt = NULL;
   LIST_CTXT_X *px_back_list_ctxt = NULL;
   LIST_NODE_X *px_curr_node_slow = NULL;
   LIST_NODE_X *px_curr_node_fast = NULL;
   LIST_INIT_PARAMS_X x_init_params = {0};

   if ((NULL == px_list_ctxt) || (NULL == ppx_front_list_ctxt)
      || (NULL == ppx_back_list_ctxt))
   {
      LIST_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_list_ctxt->px_head) && (NULL == px_list_ctxt->px_tail))
   {
      LIST_LOG_MED("List Empty: %d", px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_LIST_EMPTY;
      goto CLEAN_RETURN;
   }

   px_curr_node_slow = px_list_ctxt->px_head;
   px_curr_node_fast = px_list_ctxt->px_head->px_next;

   while (NULL != px_curr_node_fast)
   {
      px_curr_node_fast = px_curr_node_fast->px_next;

      if (NULL == px_curr_node_fast)
      {
         break;
      }

      px_curr_node_fast = px_curr_node_fast->px_next;
      px_curr_node_slow = px_curr_node_slow->px_next;
   }

   if (NULL == px_curr_node_slow->px_next)
   {
      LIST_LOG_HIGH("List has only one element: %d. Cannot Split!",
         px_list_ctxt->ui_node_count);
      e_error = eLIST_RET_SUCCESS;
      goto CLEAN_RETURN;
   }

   x_init_params.ui_list_max_elements =
      px_list_ctxt->x_init_params.ui_list_max_elements;
   e_error = list_create_private (&px_back_list_ctxt, &x_init_params);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_create_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }
   else
   {
      LIST_LOG_HIGH("Fast: %p, Slow: %p", px_curr_node_fast, px_curr_node_slow);
      px_back_list_ctxt->px_head = px_curr_node_slow->px_next;
      px_back_list_ctxt->px_tail = px_list_ctxt->px_tail;
      px_back_list_ctxt->px_head->px_prev = NULL;

      px_front_list_ctxt = px_list_ctxt;
      px_front_list_ctxt->px_tail = px_curr_node_slow;
      px_front_list_ctxt->px_tail->px_next = NULL;

      *ppx_front_list_ctxt = px_front_list_ctxt;
      *ppx_back_list_ctxt = px_back_list_ctxt;

      LIST_LOG_HIGH("List (Front) of size %d create success",
         px_front_list_ctxt->x_init_params.ui_list_max_elements);
      LIST_LOG_HIGH("List (Front) of size %d create success",
         px_back_list_ctxt->x_init_params.ui_list_max_elements);
   }

CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_sorted_merge_private (
   LIST_CTXT_X *px_list1_ctxt,
   LIST_CTXT_X *px_list2_ctxt,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_NODE_X *px_curr_list1_node = NULL;
   LIST_NODE_X *px_curr_list2_node = NULL;
   LIST_NODE_X *px_next_list2_node = NULL;

   if ((NULL == px_list1_ctxt) || (NULL == px_list2_ctxt)
      || (NULL == fn_compare_fn_cbk))
   {
      LIST_LOG_HIGH("Invalid Args");
      e_error = eLIST_RET_SUCCESS;
      goto CLEAN_RETURN;
   }

   px_list1_ctxt->x_init_params.ui_list_max_elements =
         px_list1_ctxt->x_init_params.ui_list_max_elements +
         px_list2_ctxt->x_init_params.ui_list_max_elements;

   if ((NULL == px_list1_ctxt->px_head) && (NULL == px_list1_ctxt->px_tail))
   {
      if ((NULL != px_list2_ctxt->px_head) && (NULL != px_list2_ctxt->px_tail))
      {
         px_list1_ctxt->px_head = px_list2_ctxt->px_head;
         px_list1_ctxt->px_tail = px_list2_ctxt->px_tail;
         px_list1_ctxt->ui_node_count = px_list2_ctxt->ui_node_count;
         px_list1_ctxt->x_init_params.ui_list_max_elements =
            px_list2_ctxt->x_init_params.ui_list_max_elements;
      }
      e_error = list_delete_private (px_list2_ctxt);
      if (eLIST_RET_SUCCESS != e_error)
      {
         LIST_LOG_MED("list_delete_private failed: %d", e_error);
      }
      goto CLEAN_RETURN;
   }

   if ((NULL == px_list2_ctxt->px_head) && (NULL == px_list2_ctxt->px_tail))
   {
      e_error = list_delete_private (px_list2_ctxt);
      if (eLIST_RET_SUCCESS != e_error)
      {
         LIST_LOG_MED("list_delete_private failed: %d", e_error);
      }
      goto CLEAN_RETURN;
   }

   px_curr_list1_node = px_list1_ctxt->px_head;
   px_curr_list2_node = px_list2_ctxt->px_head;

   while ((NULL != px_curr_list1_node) && (NULL != px_curr_list2_node))
   {
      e_error = fn_compare_fn_cbk (&(px_curr_list1_node->x_node_data),
         &(px_curr_list2_node->x_node_data), p_app_data);
      if (eLIST_RET_CMP_LESSER == e_error)
      {
         px_curr_list1_node = px_curr_list1_node->px_next;
         e_error = eLIST_RET_SUCCESS;
      }
      else if ((eLIST_RET_CMP_GREATER == e_error)
         || (eLIST_RET_CMP_EQUAL == e_error))
      {
         px_next_list2_node = px_curr_list2_node->px_next;

         px_curr_list2_node->px_prev = px_curr_list1_node->px_prev;
         px_curr_list2_node->px_next = px_curr_list1_node;

         px_curr_list1_node->px_prev = px_curr_list2_node;

         if (NULL != px_curr_list2_node->px_prev)
         {
            px_curr_list2_node->px_prev->px_next = px_curr_list2_node;
         }

         if (px_curr_list1_node == px_list1_ctxt->px_head)
         {
            px_list1_ctxt->px_head = px_curr_list2_node;
         }

         px_list1_ctxt->ui_node_count++;

         px_curr_list2_node = px_next_list2_node;
         px_next_list2_node = NULL;
         e_error = eLIST_RET_SUCCESS;
      }
      else
      {
         LIST_LOG_MED("fn_compare_fn_cbk failed: %d", e_error);
         break;
      }
   }
   if (eLIST_RET_SUCCESS != e_error)
   {
      goto CLEAN_RETURN;
   }

   while (NULL != px_curr_list2_node)
   {
      e_error = list_node_append_private (px_list1_ctxt, px_curr_list2_node);
      if (eLIST_RET_SUCCESS != e_error)
      {
         LIST_LOG_MED("list_node_append_private failed: %d", e_error);
         break;
      }
      px_curr_list2_node = px_curr_list2_node->px_next;
   }

   e_error = list_delete_private (px_list2_ctxt);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_delete_private failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

LIST_RET_E list_merge_sort_private (
   LIST_CTXT_X *px_list_ctxt,
   pfn_list_compare_fn_cbk fn_compare_fn_cbk,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   LIST_CTXT_X *px_front_list_ctxt = NULL;
   LIST_CTXT_X *px_back_list_ctxt = NULL;

   if (NULL == px_list_ctxt)
   {
      LIST_LOG_HIGH("Invalid Args");
      e_error = eLIST_RET_SUCCESS;
      goto CLEAN_RETURN;
   }

   e_error = list_front_back_split_private (px_list_ctxt, &px_front_list_ctxt,
      &px_back_list_ctxt);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_front_back_split_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   e_error = list_merge_sort_private (px_front_list_ctxt, fn_compare_fn_cbk,
      p_app_data);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_merge_sort_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   e_error = list_merge_sort_private (px_back_list_ctxt, fn_compare_fn_cbk,
      p_app_data);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_merge_sort_private failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   e_error = list_sorted_merge_private (px_front_list_ctxt, px_back_list_ctxt,
      fn_compare_fn_cbk, p_app_data);
   if (eLIST_RET_SUCCESS != e_error)
   {
      LIST_LOG_MED("list_sorted_merge_private failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}
