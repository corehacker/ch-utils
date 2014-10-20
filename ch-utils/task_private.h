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
 * \file   task_private.h
 *
 * \author sandeepprakash
 *
 * \date   17-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __TASK_PRIVATE_H__
#define __TASK_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define TASK_LOG_STR                       "TASK"

#define TASK_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (TASK_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define TASK_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (TASK_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define TASK_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (TASK_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define TASK_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (TASK_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/
typedef enum _TASK_STATE_E
{
   eTASK_STATE_INVALID,

   eTASK_STATE_IDLE,

   eTASK_STATE_ACTIVE,

   eTASK_STATE_MAX
} TASK_STATE_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _TASK_CTXT_X
{
   TASK_CREATE_PARAM_X  x_create_params;

   PAL_THREAD_HDL       hl_thread_hdl;

   PAL_SEM_HDL          hl_sem_hdl;

   MSGQ_HDL             hl_msgq_hdl;

   TASK_STATE_E         e_state;

   bool                 b_keep_running;
} TASK_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/

#ifdef   __cplusplus
}
#endif

#endif /* __TASK_PRIVATE_H__ */
