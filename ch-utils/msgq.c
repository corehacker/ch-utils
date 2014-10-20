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
 * \file   msgq.c
 *
 * \author sandeepprakash
 *
 * \date   25-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_q.h>
#include <ch-utils/exp_msgq.h>
#include "msgq_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/
static MSGQ_RET_E msgq_create_resources (
   MSGQ_CTXT_X *px_msgq_ctxt,
   MSGQ_INIT_PARAMS_X *px_init_params);

static MSGQ_RET_E msgq_delete_resources (
   MSGQ_CTXT_X *px_msgq_ctxt);

/****************************** LOCAL FUNCTIONS *******************************/
MSGQ_RET_E msgq_init (
   MSGQ_HDL *phl_msgq_hdl,
   MSGQ_INIT_PARAMS_X *px_init_params)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   MSGQ_RET_E e_error_pvt = eMSGQ_RET_FAILURE;
   MSGQ_CTXT_X *px_msgq_ctxt = NULL;

   if ((NULL == phl_msgq_hdl) || (NULL == px_init_params))
   {
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_msgq_ctxt = pal_malloc(sizeof(MSGQ_CTXT_X), NULL);
   if (NULL == px_msgq_ctxt)
   {
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_error = msgq_create_resources (px_msgq_ctxt, px_init_params);
   if (eMSGQ_RET_SUCCESS != e_error)
   {
      MSGQ_LOG_MED("msgq_create_resources failed: %d", e_error);
      e_error_pvt = msgq_delete_resources (px_msgq_ctxt);
      if (eMSGQ_RET_SUCCESS != e_error_pvt)
      {
         MSGQ_LOG_MED("msgq_delete_resources failed: %d", e_error);
      }
   }
   else
   {
      MSGQ_LOG_HIGH ("MSGQ of size %d create success",
         px_init_params->ui_msgq_size);
      *phl_msgq_hdl = (MSGQ_HDL) px_msgq_ctxt;
   }
CLEAN_RETURN:
   if (eMSGQ_RET_SUCCESS != e_error)
   {
      if (NULL != px_msgq_ctxt)
      {
         pal_free(px_msgq_ctxt);
         px_msgq_ctxt = NULL;
      }
   }
   return e_error;
}

static MSGQ_RET_E msgq_create_resources (
   MSGQ_CTXT_X *px_msgq_ctxt,
   MSGQ_INIT_PARAMS_X *px_init_params)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   PAL_SEM_CREATE_PARAM_X x_sem_param = {{0}};
   PAL_MUTEX_CREATE_PARAM_X x_mutex_param = {{0}};
   Q_PARAMS_X x_q_param = {0};

   if ((NULL == px_msgq_ctxt) || (NULL == px_init_params))
   {
      MSGQ_LOG_MED("Invalid Args");
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (px_init_params->ui_msgq_size > MSGQ_MAX_MSGQ_SIZE)
   {
      MSGQ_LOG_MED("Invalid Args. ui_msgq_size cannot be greater than %d"
         " (MSGQ_MAX_MSGQ_SIZE)", MSGQ_MAX_MSGQ_SIZE);
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_mutex_create (&(px_msgq_ctxt->hl_mutex_hdl), &x_mutex_param);
   if ((ePAL_RET_SUCCESS != e_pal_ret) || (NULL == px_msgq_ctxt->hl_mutex_hdl))
   {
      MSGQ_LOG_MED("pal_mutex_create failed: %d", e_pal_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_sem_param.ui_initial_count = 0;
   x_sem_param.ui_max_count = px_init_params->ui_msgq_size;
   e_pal_ret = pal_sem_create (&(px_msgq_ctxt->hl_rd_sem_hdl), &x_sem_param);
   if ((ePAL_RET_SUCCESS != e_pal_ret) || (NULL == px_msgq_ctxt->hl_rd_sem_hdl))
   {
      MSGQ_LOG_MED("pal_sem_create failed: %d", e_pal_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_sem_param.ui_initial_count = px_init_params->ui_msgq_size;
   x_sem_param.ui_max_count = px_init_params->ui_msgq_size;
   e_pal_ret = pal_sem_create (&(px_msgq_ctxt->hl_wr_sem_hdl), &x_sem_param);
   if ((ePAL_RET_SUCCESS != e_pal_ret) || (NULL == px_msgq_ctxt->hl_wr_sem_hdl))
   {
      MSGQ_LOG_MED("pal_sem_create failed: %d", e_pal_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_q_param.ui_max_elements = px_init_params->ui_msgq_size;
   e_q_ret = q_init (&(px_msgq_ctxt->hl_q_hdl), &x_q_param);
   if ((eQ_RET_SUCCESS != e_q_ret) || (NULL == px_msgq_ctxt->hl_q_hdl))
   {
      MSGQ_LOG_MED("q_init failed: %d", e_q_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
   }
   else
   {
      e_error = eMSGQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

static MSGQ_RET_E msgq_delete_resources (
   MSGQ_CTXT_X *px_msgq_ctxt)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;

   if (NULL == px_msgq_ctxt)
   {
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL != px_msgq_ctxt->hl_q_hdl)
   {
      if (NULL != px_msgq_ctxt->hl_mutex_hdl)
      {
         e_pal_ret = pal_mutex_lock (px_msgq_ctxt->hl_mutex_hdl);
         if (ePAL_RET_SUCCESS != e_pal_ret)
         {
            MSGQ_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         }
      }

      e_q_ret = q_deinit(px_msgq_ctxt->hl_q_hdl);
      if (eQ_RET_SUCCESS != e_q_ret)
      {
         MSGQ_LOG_MED("q_deinit failed: %d", e_q_ret);
      }
      px_msgq_ctxt->hl_q_hdl = NULL;

      if (NULL != px_msgq_ctxt->hl_mutex_hdl)
      {
         e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
         if (ePAL_RET_SUCCESS != e_pal_ret)
         {
            MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         }
      }
   }

   if (NULL != px_msgq_ctxt->hl_wr_sem_hdl)
   {
      e_pal_ret = pal_sem_destroy(px_msgq_ctxt->hl_wr_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_destroy failed: %d", e_pal_ret);
      }
      px_msgq_ctxt->hl_wr_sem_hdl = NULL;
   }

   if (NULL != px_msgq_ctxt->hl_rd_sem_hdl)
   {
      e_pal_ret = pal_sem_destroy(px_msgq_ctxt->hl_rd_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_destroy failed: %d", e_pal_ret);
      }
      px_msgq_ctxt->hl_rd_sem_hdl = NULL;
   }

   if (NULL != px_msgq_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_destroy(px_msgq_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_mutex_destroy failed: %d", e_pal_ret);
      }
      px_msgq_ctxt->hl_mutex_hdl = NULL;
   }
   e_error = eMSGQ_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

MSGQ_RET_E msgq_deinit (
   MSGQ_HDL hl_msgq_hdl)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   MSGQ_CTXT_X *px_msgq_ctxt = NULL;

   if (NULL == hl_msgq_hdl)
   {
      e_error = eMSGQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_msgq_ctxt = (MSGQ_CTXT_X *) hl_msgq_hdl;

   e_error = msgq_delete_resources(px_msgq_ctxt);
   if (eMSGQ_RET_SUCCESS != e_error)
   {
      MSGQ_LOG_MED("msgq_delete_resources failed: %d", e_error);
   }

   pal_free(px_msgq_ctxt);
   px_msgq_ctxt = NULL;
   e_error = eMSGQ_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

MSGQ_RET_E msgq_add_msg (
   MSGQ_HDL hl_msgq_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   MSGQ_CTXT_X *px_msgq_ctxt = NULL;
   Q_NODE_DATA_X x_q_data = {NULL};

   if (NULL == hl_msgq_hdl)
   {
      e_error = eMSGQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   if (NULL == px_data)
   {
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_data->p_data) || (0 == px_data->ui_data_size))
   {
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_msgq_ctxt = (MSGQ_CTXT_X *) hl_msgq_hdl;

   /*
    * 1. Wait for write semaphone.
    * 2. Acquire mutex lock.
    * 3. Add the message to queue.
    * 4. Release mutex lock.
    * 5. Signal the read semaphore.
    */
   e_pal_ret = pal_sem_get (px_msgq_ctxt->hl_wr_sem_hdl, ui_timeout);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      if (ePAL_RET_SEM_IN_USE == e_pal_ret)
      {
         MSGQ_LOG_FULL("pal_sem_get timedout: %d", e_pal_ret);
         e_error = eMSGQ_RET_OP_TIMEDOUT;
      }
      else
      {
         MSGQ_LOG_MED("pal_sem_get failed: %d", e_pal_ret);
         e_error = eMSGQ_RET_RESOURCE_FAILURE;
      }
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_mutex_lock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_wr_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   x_q_data.p_data = px_data->p_data;
   x_q_data.ui_data_size = px_data->ui_data_size;

   e_q_ret = q_enq (px_msgq_ctxt->hl_q_hdl, &x_q_data);
   if (eQ_RET_SUCCESS != e_q_ret)
   {
      MSGQ_LOG_MED("q_enq failed: %d", e_q_ret);
      e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
      }

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_wr_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }

      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_wr_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }

      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_rd_sem_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_wr_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
   }
   else
   {
      e_error = eMSGQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

MSGQ_RET_E msgq_get_msg (
   MSGQ_HDL hl_msgq_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   MSGQ_CTXT_X *px_msgq_ctxt = NULL;
   Q_NODE_DATA_X x_q_data = {NULL};

   if (NULL == hl_msgq_hdl)
   {
      e_error = eMSGQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   if (NULL == px_data)
   {
      e_error = eMSGQ_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_msgq_ctxt = (MSGQ_CTXT_X *) hl_msgq_hdl;

   /*
    * 1. Wait for read semaphone.
    * 2. Acquire mutex lock.
    * 3. Get the message from queue.
    * 4. Release mutex lock.
    * 5. Signal the write semaphore.
    */
   e_pal_ret = pal_sem_get (px_msgq_ctxt->hl_rd_sem_hdl, ui_timeout);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      if (ePAL_RET_SEM_IN_USE == e_pal_ret)
      {
         MSGQ_LOG_FULL("pal_sem_get timedout: %d", e_pal_ret);
         e_error = eMSGQ_RET_OP_TIMEDOUT;
      }
      else
      {
         MSGQ_LOG_MED("pal_sem_get failed: %d", e_pal_ret);
         e_error = eMSGQ_RET_RESOURCE_FAILURE;
      }
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_mutex_lock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_rd_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }

      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_q_ret = q_deq (px_msgq_ctxt->hl_q_hdl, &x_q_data);
   if (eQ_RET_SUCCESS != e_q_ret)
   {
      MSGQ_LOG_MED("q_deq failed: %d", e_q_ret);

      e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
      }

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_rd_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }

      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_rd_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }

      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_wr_sem_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);

      e_pal_ret = pal_sem_put (px_msgq_ctxt->hl_rd_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      }
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
   }
   else
   {
      px_data->p_data = x_q_data.p_data;
      px_data->ui_data_size = x_q_data.ui_data_size;
      e_error = eMSGQ_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

MSGQ_RET_E msgq_get_filled_q_size (
   MSGQ_HDL hl_msgq_hdl,
   uint32_t *pui_filled_q_size)
{
   MSGQ_RET_E e_error = eMSGQ_RET_FAILURE;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   MSGQ_CTXT_X *px_msgq_ctxt = NULL;

   if ((NULL == hl_msgq_hdl) || (NULL == pui_filled_q_size))
   {
      e_error = eMSGQ_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   *pui_filled_q_size = 0;
   px_msgq_ctxt = (MSGQ_CTXT_X *) hl_msgq_hdl;

   e_pal_ret = pal_mutex_lock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   e_q_ret = q_get_filled_size (px_msgq_ctxt->hl_q_hdl, pui_filled_q_size);
   if (eQ_RET_SUCCESS != e_q_ret)
   {
      MSGQ_LOG_MED("q_get_filled_size failed: %d", e_q_ret);
      e_error = eMSGQ_RET_RESOURCE_FAILURE;
   }
   e_pal_ret = pal_mutex_unlock (px_msgq_ctxt->hl_mutex_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
   }
CLEAN_RETURN:
   return e_error;
}
