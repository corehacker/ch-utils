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
 * \file   q.c
 *
 * \author sandeepprakash
 *
 * \date   24-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_q.h>
#include "q_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
Q_RET_E q_init (
   Q_HDL *phl_q_hdl,
   Q_PARAMS_X *px_init_params)
{
   Q_RET_E e_error = eQ_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   Q_CTXT_X *px_q_ctxt = NULL;
   LIST_INIT_PARAMS_X x_list_params = {0};

   if ((NULL == phl_q_hdl) || (NULL == px_init_params))
   {
      e_error = eQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_q_ctxt = pal_malloc(sizeof(Q_CTXT_X), NULL);
   if (NULL == px_q_ctxt)
   {
      e_error = eQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_list_params.ui_list_max_elements = px_init_params->ui_max_elements;
   e_list_ret = list_create(&(px_q_ctxt->hl_list_hdl), &x_list_params);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      e_error = eQ_RET_RESOURCE_FAILURE;
   }
   else
   {
      Q_LOG_HIGH("Q of size %d create success",
         px_init_params->ui_max_elements);
      *phl_q_hdl = (Q_HDL) px_q_ctxt;
      e_error = eQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   if (eQ_RET_SUCCESS != e_error)
   {
      if (NULL != px_q_ctxt)
      {
         pal_free(px_q_ctxt);
         px_q_ctxt = NULL;
      }
   }
   return e_error;
}

Q_RET_E q_deinit (
   Q_HDL hl_q_hdl)
{
   Q_RET_E e_error = eQ_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   Q_CTXT_X *px_q_ctxt = NULL;

   if (NULL == hl_q_hdl)
   {
      e_error = eQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_q_ctxt = (Q_CTXT_X *) hl_q_hdl;

   if (NULL != px_q_ctxt->hl_list_hdl)
   {
      e_list_ret = list_delete(px_q_ctxt->hl_list_hdl);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {

      }
      px_q_ctxt->hl_list_hdl = NULL;
   }
   pal_free(px_q_ctxt);
   px_q_ctxt = NULL;
   e_error = eQ_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

Q_RET_E q_deq (
   Q_HDL hl_q_hdl,
   Q_NODE_DATA_X *px_node_data)
{
   Q_RET_E e_error = eQ_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   Q_CTXT_X *px_q_ctxt = NULL;
   LIST_NODE_DATA_X x_node_data = {NULL};

   if ((NULL == hl_q_hdl) || (NULL == px_node_data))
   {
      e_error = eQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_q_ctxt = (Q_CTXT_X *) hl_q_hdl;

   e_list_ret = list_node_delete_at_head(px_q_ctxt->hl_list_hdl,
      &x_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         e_error = eQ_RET_Q_EMPTY;
      }
      else
      {
         e_error = eQ_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      px_node_data->p_data = x_node_data.p_data;
      px_node_data->ui_data_size = x_node_data.ui_data_size;
      e_error = eQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

Q_RET_E q_enq (
   Q_HDL hl_q_hdl,
   Q_NODE_DATA_X *px_node_data)
{
   Q_RET_E e_error = eQ_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   Q_CTXT_X *px_q_ctxt = NULL;
   LIST_NODE_DATA_X x_node_data = {NULL};

   if ((NULL == hl_q_hdl) || (NULL == px_node_data))
   {
      e_error = eQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_q_ctxt = (Q_CTXT_X *) hl_q_hdl;

   x_node_data.p_data = px_node_data->p_data;
   x_node_data.ui_data_size = px_node_data->ui_data_size;
   e_list_ret = list_node_append(px_q_ctxt->hl_list_hdl, &x_node_data);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_FULL == e_list_ret)
      {
         e_error = eQ_RET_Q_FULL;
      }
      else
      {
         e_error = eQ_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      e_error = eQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

Q_RET_E q_get_filled_size (
   Q_HDL hl_q_hdl,
   uint32_t *pui_filled_q_size)
{
   Q_RET_E e_error = eQ_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   Q_CTXT_X *px_q_ctxt = NULL;

   if ((NULL == hl_q_hdl) || (NULL == pui_filled_q_size))
   {
      e_error = eQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   *pui_filled_q_size = 0;
   px_q_ctxt = (Q_CTXT_X *) hl_q_hdl;

   e_list_ret = list_get_len (px_q_ctxt->hl_list_hdl, pui_filled_q_size);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      e_error = eQ_RET_RESOURCE_FAILURE;
   }
   else
   {
      e_error = eQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}
