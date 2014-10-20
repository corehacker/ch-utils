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
 * \file   exp_q.h
 *
 * \author sandeepprakash
 *
 * \date   24-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_Q_H__
#define __EXP_Q_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/
typedef enum _Q_RET_E
{
   eQ_RET_SUCCESS                  = 0x00000000,

   eQ_RET_FAILURE,

   eQ_RET_INVALID_ARGS,

   eQ_RET_INVALID_HANDLE,

   eQ_RET_RESOURCE_FAILURE,

   eQ_RET_Q_FULL,

   eQ_RET_Q_EMPTY,

   eQ_RET_MAX,
} Q_RET_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct Q_CTXT_X            *Q_HDL;

typedef struct _Q_PARAMS_X
{
   uint32_t ui_max_elements;
} Q_PARAMS_X;

typedef struct _Q_NODE_DATA_X
{
   void *p_data;

   uint32_t ui_data_size;
} Q_NODE_DATA_X;

/***************************** FUNCTION PROTOTYPES ****************************/
Q_RET_E q_init (
   Q_HDL *phl_q_hdl,
   Q_PARAMS_X *px_init_params);

Q_RET_E q_deinit (
   Q_HDL hl_q_hdl);

Q_RET_E q_deq (
   Q_HDL hl_q_hdl,
   Q_NODE_DATA_X *px_node_data);

Q_RET_E q_enq (
   Q_HDL hl_q_hdl,
   Q_NODE_DATA_X *px_node_data);

Q_RET_E q_get_filled_size (
   Q_HDL hl_q_hdl,
   uint32_t *pui_filled_q_size);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_Q_H__ */
