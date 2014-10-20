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
 * \file   msgq_private.h
 *
 * \author sandeepprakash
 *
 * \date   25-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __MSGQ_PRIVATE_H__
#define __MSGQ_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define MSGQ_LOG_STR                         "MSGQ"

#define MSGQ_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (MSGQ_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define MSGQ_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (MSGQ_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define MSGQ_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (MSGQ_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define MSGQ_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (MSGQ_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _MSGQ_CTXT_X
{
   MSGQ_INIT_PARAMS_X x_init_params;

   Q_HDL hl_q_hdl;

   PAL_SEM_HDL hl_rd_sem_hdl;

   PAL_SEM_HDL hl_wr_sem_hdl;

   PAL_MUTEX_HDL hl_mutex_hdl;
} MSGQ_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/

#ifdef   __cplusplus
}
#endif

#endif /* __MSGQ_PRIVATE_H__ */
