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
 * \file   task.c
 *
 * \author sandeepprakash
 *
 * \date   17-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_msgq.h>
#include <ch-utils/exp_task.h>
#include "task_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
TASK_RET_E task_create (
   TASK_HDL *phl_task_hdl,
   TASK_CREATE_PARAM_X  *px_create_params)
{
   TASK_RET_E                  e_error          = eTASK_RET_FAILURE;
   PAL_RET_E                   e_pal_ret        = ePAL_RET_FAILURE;
   MSGQ_RET_E                  e_msgq_ret       = eMSGQ_RET_FAILURE;
   TASK_CTXT_X                *px_task_ctxt     = NULL;
   PAL_SEM_CREATE_PARAM_X      x_sem_param      = {{0}};
   MSGQ_INIT_PARAMS_X          x_msgq_param     = {0};

   if ((NULL == phl_task_hdl) || (NULL == px_create_params))
   {
      TASK_LOG_MED("Invalid Args");
      e_error = eTASK_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL == px_create_params->fn_task_routine)
   {
      TASK_LOG_MED("Invalid Args");
      e_error = eTASK_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (px_create_params->ui_msgq_size > TASK_MAX_MSGQ_SIZE)
   {
      TASK_LOG_MED("Invalid Args. ui_msgq_size cannot be greater than %d"
         " (MSGQ_MAX_MSGQ_SIZE)", TASK_MAX_MSGQ_SIZE);
      e_error = eTASK_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = pal_malloc (sizeof(TASK_CTXT_X), NULL);
   if (NULL == px_task_ctxt)
   {
      TASK_LOG_MED("pal_malloc failed: %p", px_task_ctxt);
      e_error = eTASK_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   (void) pal_memmove(&(px_task_ctxt->x_create_params), px_create_params,
      sizeof(px_task_ctxt->x_create_params));

   if (true == px_create_params->b_msgq_needed)
   {
      if (0 == px_create_params->ui_msgq_size)
      {
         TASK_LOG_MED("Invalid Args");
         e_error = eTASK_RET_INVALID_ARGS;
         goto CLEAN_RETURN;
      }
      x_msgq_param.ui_msgq_size = px_create_params->ui_msgq_size;
      e_msgq_ret = msgq_init (&(px_task_ctxt->hl_msgq_hdl), &x_msgq_param);
      if ((eMSGQ_RET_SUCCESS != e_msgq_ret)
         || (NULL == px_task_ctxt->hl_msgq_hdl))
      {
         TASK_LOG_MED("msgq_init failed: %d", e_msgq_ret);
         e_error = eTASK_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      else
      {
         TASK_LOG_MED("msgq_init success");
      }
   }

   x_sem_param.ui_initial_count = 0;
   x_sem_param.ui_max_count = 1;
   e_pal_ret = pal_sem_create(&(px_task_ctxt->hl_sem_hdl), &x_sem_param);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      TASK_LOG_MED("pal_sem_create failed: %d", e_pal_ret);
      e_error = eTASK_RET_RESOURCE_FAILURE;
   }
   else
   {
      TASK_LOG_HIGH("pal_sem_create success");
      *phl_task_hdl = (TASK_HDL) px_task_ctxt;
      e_error = eTASK_RET_SUCCESS;
   }
CLEAN_RETURN:
   if (eTASK_RET_SUCCESS != e_error)
   {
      if (NULL != px_task_ctxt)
      {
         if (NULL != px_task_ctxt->hl_sem_hdl)
         {
            e_pal_ret = pal_sem_destroy(px_task_ctxt->hl_sem_hdl);
            if (ePAL_RET_SUCCESS != e_pal_ret)
            {
               TASK_LOG_MED("pal_sem_destroy failed: %d", e_pal_ret);
            }
            px_task_ctxt->hl_sem_hdl = NULL;
         }

         if (NULL != px_task_ctxt->hl_msgq_hdl)
         {
            e_msgq_ret = msgq_deinit(px_task_ctxt->hl_msgq_hdl);
            if (eMSGQ_RET_SUCCESS != e_msgq_ret)
            {
               TASK_LOG_MED("msgq_deinit failed: %d", e_msgq_ret);
            }
            px_task_ctxt->hl_msgq_hdl = NULL;
         }
         pal_free (px_task_ctxt);
         px_task_ctxt = NULL;
      }
   }
   return e_error;
}

TASK_RET_E task_start (
   TASK_HDL hl_task_hdl)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;
   PAL_THREAD_CREATE_PARAMS_X  x_thread_param   = {{0}};
   PAL_RET_E          e_pal_ret        = ePAL_RET_FAILURE;

   if (NULL == hl_task_hdl)
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;

   px_task_ctxt->b_keep_running = true;
   px_task_ctxt->e_state = eTASK_STATE_IDLE;

   x_thread_param.fn_start_routine =
      px_task_ctxt->x_create_params.fn_task_routine;
   x_thread_param.p_app_data = px_task_ctxt->x_create_params.p_app_data;
   x_thread_param.ui_stack_size_bytes =
      px_task_ctxt->x_create_params.ui_thread_stack_size;
   e_pal_ret = pal_thread_create (&(px_task_ctxt->hl_thread_hdl),
      &x_thread_param);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      TASK_LOG_MED("pal_thread_create failed: %d", e_pal_ret);
      e_error = eTASK_RET_RESOURCE_FAILURE;
   }
   else
   {
      TASK_LOG_HIGH("pal_thread_create success");
      e_error = eTASK_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}


TASK_RET_E task_delete (
   TASK_HDL hl_task_hdl)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   MSGQ_RET_E         e_msgq_ret       = eMSGQ_RET_FAILURE;
   PAL_RET_E          e_pal_ret        = ePAL_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;

   if (NULL == hl_task_hdl)
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;

   px_task_ctxt->b_keep_running = false;

   e_pal_ret = pal_sem_get (px_task_ctxt->hl_sem_hdl, PAL_TIME_WAIT_INFINITE);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      TASK_LOG_MED("pal_sem_get failed: %d", e_pal_ret);
   }

   if (NULL != px_task_ctxt->hl_thread_hdl)
   {
      e_pal_ret = pal_thread_delete (px_task_ctxt->hl_thread_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         TASK_LOG_MED("pal_thread_delete failed: %d", e_pal_ret);
      }
      px_task_ctxt->hl_thread_hdl = NULL;
   }

   if (NULL != px_task_ctxt->hl_sem_hdl)
   {
      e_pal_ret = pal_sem_destroy (px_task_ctxt->hl_sem_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         TASK_LOG_MED("pal_sem_destroy failed: %d", e_pal_ret);
      }
      px_task_ctxt->hl_sem_hdl = NULL;
   }

   if (NULL != px_task_ctxt->hl_msgq_hdl)
   {
      e_msgq_ret = msgq_deinit(px_task_ctxt->hl_msgq_hdl);
      if (eMSGQ_RET_SUCCESS != e_msgq_ret)
      {
         TASK_LOG_MED("msgq_deinit failed: %d", e_msgq_ret);
      }
      px_task_ctxt->hl_msgq_hdl = NULL;
   }

   pal_free (px_task_ctxt);
   px_task_ctxt = NULL;
   TASK_LOG_HIGH("Task delete success");
   e_error = eTASK_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

TASK_RET_E task_notify_exit (
   TASK_HDL hl_task_hdl)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;
   PAL_RET_E          e_pal_ret        = ePAL_RET_FAILURE;

   if (NULL == hl_task_hdl)
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;

   e_pal_ret = pal_sem_put(px_task_ctxt->hl_sem_hdl);
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      TASK_LOG_MED("pal_sem_put failed: %d", e_pal_ret);
      e_error = eTASK_RET_RESOURCE_FAILURE;
   }
   else
   {
      TASK_LOG_HIGH("pal_sem_put success");
      e_error = eTASK_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

bool task_is_in_loop (
   TASK_HDL hl_task_hdl)
{
   bool b_is_in_loop = false;
   TASK_CTXT_X       *px_task_ctxt     = NULL;

   if (NULL != hl_task_hdl)
   {
      px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;
      b_is_in_loop = px_task_ctxt->b_keep_running;
   }
   else
   {
      TASK_LOG_MED("Invalid Handle");
   }
   return b_is_in_loop;
}

TASK_RET_E task_exit_loop (
   TASK_HDL hl_task_hdl)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;

   if (NULL != hl_task_hdl)
   {
      px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;
      px_task_ctxt->b_keep_running = false;
      e_error = eTASK_RET_SUCCESS;
   }
   else
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
   }
   return e_error;
}

TASK_RET_E task_get_msg_from_q (
   TASK_HDL hl_task_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   MSGQ_RET_E         e_msgq_ret       = eMSGQ_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;

   if (NULL == hl_task_hdl)
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   if (NULL == px_data)
   {
      e_error = eTASK_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;

   if (NULL == px_task_ctxt->hl_msgq_hdl)
   {
      TASK_LOG_MED("Invalid Handle. Probably MsgQ is disabled during init.");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   e_msgq_ret = msgq_get_msg (px_task_ctxt->hl_msgq_hdl, px_data, ui_timeout);
   if (eMSGQ_RET_SUCCESS != e_msgq_ret)
   {
      if (eMSGQ_RET_OP_TIMEDOUT == e_msgq_ret)
      {
         TASK_LOG_FULL("msgq_get_msg timedout: %d", e_msgq_ret);
         e_error = eTASK_RET_OPERATION_TIMEDOUT;
      }
      else
      {
         TASK_LOG_MED("msgq_get_msg failed: %d", e_msgq_ret);
         e_error = eTASK_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      e_error = eTASK_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

TASK_RET_E task_add_msg_to_q (
   TASK_HDL hl_task_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout)
{
   TASK_RET_E         e_error          = eTASK_RET_FAILURE;
   MSGQ_RET_E         e_msgq_ret       = eMSGQ_RET_FAILURE;
   TASK_CTXT_X       *px_task_ctxt     = NULL;

   if (NULL == hl_task_hdl)
   {
      TASK_LOG_MED("Invalid Handle");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   if (NULL == px_data)
   {
      e_error = eTASK_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_task_ctxt = (TASK_CTXT_X *) hl_task_hdl;

   if (NULL == px_task_ctxt->hl_msgq_hdl)
   {
      TASK_LOG_MED("Invalid Handle. Probably MsgQ is disabled during init.");
      e_error = eTASK_RET_INVALID_HANDLE;
      goto CLEAN_RETURN;
   }

   e_msgq_ret = msgq_add_msg (px_task_ctxt->hl_msgq_hdl, px_data, ui_timeout);
   if (eMSGQ_RET_SUCCESS != e_msgq_ret)
   {
      if (eMSGQ_RET_OP_TIMEDOUT == e_msgq_ret)
      {
         TASK_LOG_FULL("msgq_add_msg timedout: %d", e_msgq_ret);
         e_error = eTASK_RET_OPERATION_TIMEDOUT;
      }
      else
      {
         TASK_LOG_MED("msgq_add_msg failed: %d", e_msgq_ret);
         e_error = eTASK_RET_RESOURCE_FAILURE;
      }
   }
   else
   {
      e_error = eTASK_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}
