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
 * \file   q_private.h
 *
 * \author sandeepprakash
 *
 * \date   24-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __Q_PRIVATE_H__
#define __Q_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define Q_LOG_STR                       "QUEU"

#define Q_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (Q_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define Q_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (Q_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define Q_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (Q_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define Q_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (Q_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _Q_CTXT_X
{
   LIST_HDL hl_list_hdl;
} Q_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/

#ifdef   __cplusplus
}
#endif

#endif /* __Q_PRIVATE_H__ */
