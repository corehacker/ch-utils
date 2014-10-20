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
 * \file   hashmap_private_cbks.c
 *
 * \author sandeepprakash
 *
 * \date   Mar 3, 2014
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
LIST_RET_E hm_list_for_all_cbk (
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_FOR_EACH_METADATA_X *px_metadata = NULL;
   HM_NODE_DATA_X *px_node_data = NULL;
   HM_RET_E e_hm_error = eHM_RET_FAILURE;

   if ((NULL == px_curr_node_data) || (NULL == p_app_data))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_curr_node_data->p_data)
      || (0 == px_curr_node_data->ui_data_size)
      || (px_curr_node_data->ui_data_size != sizeof(*px_node_data)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_metadata = p_app_data;
   if ((NULL == px_metadata->px_hm_ctxt)
      || (NULL == px_metadata->fn_hm_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_node_data = (HM_NODE_DATA_X *) px_curr_node_data->p_data;
   e_hm_error = px_metadata->fn_hm_for_each_cbk (px_node_data,
      px_metadata->p_hm_for_each_app_data);
   if (eHM_RET_SUCCESS != e_hm_error)
   {
      HM_LOG_MED("px_metadata->fn_for_each_cbk failed: %d", e_hm_error);
      e_list_ret = eLIST_RET_RESOURCE_FAILURE;
   }
   else
   {
      e_list_ret = eLIST_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_list_ret;
}

LIST_RET_E hm_print_nodes_for_all_cbk (
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_NODE_DATA_X *px_node_data = NULL;

   if ((NULL == px_curr_node_data) || (NULL == p_app_data))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_curr_node_data->p_data)
      || (0 == px_curr_node_data->ui_data_size)
      || (px_curr_node_data->ui_data_size != sizeof(HM_NODE_DATA_X)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_node_data = px_curr_node_data->p_data;

   switch (px_node_data->e_hm_key_type)
   {
      case eHM_KEY_TYPE_INT:
      {
         printf ("\t%d, %p\n", px_node_data->u_hm_key.ui_uint_key,
            px_node_data->p_data);
         break;
      }
      case eHM_KEY_TYPE_STRING:
      {
         printf ("\t%s, %p\n", px_node_data->u_hm_key.puc_str_key,
            px_node_data->p_data);
         break;
      }
      default:
      {
         break;
      }
   }
   e_list_ret = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_list_ret;
}

LIST_RET_E hm_list_search_cbk (
   LIST_NODE_DATA_X *px_search_node_data,
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_NODE_DATA_X *px_hm_search_node_data = NULL;
   HM_NODE_DATA_X *px_hm_curr_node_data = NULL;
   int32_t i_cmp_ret = -1;

   if ((NULL == px_curr_node_data) || (NULL == p_app_data))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_search_node_data->p_data)
      || (0 == px_search_node_data->ui_data_size)
      || (px_search_node_data->ui_data_size != sizeof(HM_NODE_DATA_X)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_curr_node_data->p_data)
      || (0 == px_curr_node_data->ui_data_size)
      || (px_curr_node_data->ui_data_size != sizeof(HM_NODE_DATA_X)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_hm_search_node_data = px_search_node_data->p_data;
   px_hm_curr_node_data = px_curr_node_data->p_data;

   e_list_ret = eLIST_RET_LIST_NODE_NOT_FOUND;
   switch (px_hm_search_node_data->e_hm_key_type)
   {
      case eHM_KEY_TYPE_INT:
      {
         if (px_hm_search_node_data->u_hm_key.ui_uint_key ==
               px_hm_curr_node_data->u_hm_key.ui_uint_key)
         {
            e_list_ret = eLIST_RET_LIST_NODE_FOUND;
         }
         break;
      }
      case eHM_KEY_TYPE_STRING:
      {
         i_cmp_ret = pal_strcmp (px_hm_search_node_data->u_hm_key.puc_str_key,
            px_hm_curr_node_data->u_hm_key.puc_str_key);
         if (0 == i_cmp_ret)
         {
            e_list_ret = eLIST_RET_LIST_NODE_FOUND;
         }
         break;
      }
      default:
      {
         break;
      }
   }
CLEAN_RETURN:
   return e_list_ret;
}

LIST_RET_E hm_list_compare_fn_cbk(
   LIST_NODE_DATA_X *px_node_a_data,
   LIST_NODE_DATA_X *px_node_b_data,
   void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_COMPARE_METADATA_X *px_metadata = NULL;
   HM_NODE_DATA_X *px_node_data_a = NULL;
   HM_NODE_DATA_X *px_node_data_b = NULL;
   HM_RET_E e_hm_error = eHM_RET_FAILURE;

   if ((NULL == px_node_a_data) || (NULL == px_node_b_data)
      || (NULL == p_app_data))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_node_a_data->p_data) || (0 == px_node_a_data->ui_data_size)
      || (px_node_a_data->ui_data_size != sizeof(*px_node_data_a)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_node_b_data->p_data) || (0 == px_node_b_data->ui_data_size)
      || (px_node_b_data->ui_data_size != sizeof(*px_node_data_b)))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_metadata = p_app_data;
   if ((NULL == px_metadata->px_hm_ctxt)
      || (NULL == px_metadata->fn_hm_compare_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_node_data_a = (HM_NODE_DATA_X *) px_node_a_data->p_data;
   px_node_data_b = (HM_NODE_DATA_X *) px_node_b_data->p_data;
   e_hm_error = px_metadata->fn_hm_compare_cbk (px_node_data_a, px_node_data_b,
      px_metadata->p_hm_compare_app_data);
   HM_LOG_HIGH("px_metadata->fn_hm_compare_cbk returned: %d", e_hm_error);
   if (eHM_RET_CMP_LESSER == e_hm_error)
   {
      e_list_ret = eLIST_RET_CMP_LESSER;
   }
   else if (eHM_RET_CMP_GREATER == e_hm_error)
   {
      e_list_ret = eLIST_RET_CMP_GREATER;
   }
   else if (eHM_RET_CMP_EQUAL == e_hm_error)
   {
      e_list_ret = eLIST_RET_CMP_EQUAL;
   }
   else
   {
      e_list_ret = eLIST_RET_RESOURCE_FAILURE;
   }
CLEAN_RETURN:
   return e_list_ret;
}
