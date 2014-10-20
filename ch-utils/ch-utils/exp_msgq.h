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
 * \file   exp_msgq.h
 *
 * \author sandeepprakash
 *
 * \date   25-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_MSGQ_H__
#define __EXP_MSGQ_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define MSGQ_MAX_MSGQ_SIZE                (PAL_SEM_VALUE_MAX)

/******************************** ENUMERATIONS ********************************/
typedef enum _MSGQ_RET_E
{
   eMSGQ_RET_SUCCESS                      = 0x00000000,

   eMSGQ_RET_FAILURE,

   eMSGQ_RET_INVALID_ARGS,

   eMSGQ_RET_INVALID_HANDLE,

   eMSGQ_RET_RESOURCE_FAILURE,

   eMSGQ_RET_OP_TIMEDOUT,

   eMSGQ_RET_Q_FULL,

   eMSGQ_RET_Q_EMPTY,

   eMSGQ_RET_MAX
} MSGQ_RET_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct MSGQ_CTXT_X             *MSGQ_HDL;

typedef struct _MSGQ_INIT_PARAMS_X
{
   uint32_t ui_msgq_size;
} MSGQ_INIT_PARAMS_X;

typedef struct _MSGQ_DATA_X
{
   void *p_data;

   uint32_t ui_data_size;
} MSGQ_DATA_X;

/***************************** FUNCTION PROTOTYPES ****************************/
MSGQ_RET_E msgq_init (
   MSGQ_HDL *phl_msgq_hdl,
   MSGQ_INIT_PARAMS_X *px_init_params);

MSGQ_RET_E msgq_deinit (
   MSGQ_HDL hl_msgq_hdl);

MSGQ_RET_E msgq_add_msg (
   MSGQ_HDL hl_msgq_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout);

MSGQ_RET_E msgq_get_msg (
   MSGQ_HDL hl_msgq_hdl,
   MSGQ_DATA_X *px_data,
   uint32_t ui_timeout);

MSGQ_RET_E msgq_get_filled_q_size (
   MSGQ_HDL hl_msgq_hdl,
   uint32_t *pui_filled_q_size);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_MSGQ_H__ */
