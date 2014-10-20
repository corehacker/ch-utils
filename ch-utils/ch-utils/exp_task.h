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
 * \file   exp_task.h
 *
 * \author sandeepprakash
 *
 * \date   17-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_TASK_H__
#define __EXP_TASK_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define TASK_NAME_STR_MAX_LEN          (PAL_THREAD_NAME_MAX_STR_LEN)

#define TASK_MAX_MSGQ_SIZE             (MSGQ_MAX_MSGQ_SIZE)

/******************************** ENUMERATIONS ********************************/
typedef enum _TASK_RET_E
{
   eTASK_RET_SUCCESS,

   eTASK_RET_FAILURE,

   eTASK_RET_INVALID_ARGS,

   eTASK_RET_INVALID_HANDLE,

   eTASK_RET_RESOURCE_FAILURE,

   eTASK_RET_OPERATION_TIMEDOUT,

   eTASK_RET_MAX,
} TASK_RET_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct TASK_CTXT_X        *TASK_HDL;

typedef struct _TASK_CREATE_PARAM_X
{
   uint8_t                       uca_task_name_str[TASK_NAME_STR_MAX_LEN];

   pfn_pal_thread_start_routine  fn_task_routine;

   void                          *p_app_data;

   uint32_t                       ui_thread_stack_size;

   bool                           b_msgq_needed;

   uint32_t                       ui_msgq_size;
} TASK_CREATE_PARAM_X;
/***************************** FUNCTION PROTOTYPES ****************************/
TASK_RET_E task_create (
   TASK_HDL *phl_task_hdl,
   TASK_CREATE_PARAM_X  *px_create_params);

TASK_RET_E task_start (
   TASK_HDL hl_task_hdl);

TASK_RET_E task_delete (
   TASK_HDL hl_task_hdl);

TASK_RET_E task_notify_exit (
   TASK_HDL hl_task_hdl);

bool task_is_in_loop (
   TASK_HDL hl_task_hdl);

TASK_RET_E task_exit_loop (
   TASK_HDL hl_task_hdl);

TASK_RET_E task_get_msg_from_q (
   TASK_HDL hl_task_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout);

TASK_RET_E task_add_msg_to_q (
   TASK_HDL hl_task_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_TASK_H__ */
