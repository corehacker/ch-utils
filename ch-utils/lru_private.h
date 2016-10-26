/*******************************************************************************
 *  Repository for C modules.
 *  Copyright (C) 2016 Sandeep Prakash
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
 * Copyright (c) 2016, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   lru_private.h
 *
 * \author corehacker
 *
 * \date   Oct 21, 2016
 *
 * \brief  
 *
 ******************************************************************************/

#ifndef __CH_UTILS_LRU_PRIVATE_H__
#define __CH_UTILS_LRU_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LRU_LOG_STR                       "LRU"

#define LRU_LOG_LOW(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_LOW (LRU_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LRU_LOG_MED(format,...)                                              \
do                                                                            \
{                                                                             \
   LOG_MED (LRU_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,               \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LRU_LOG_HIGH(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_HIGH (LRU_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

#define LRU_LOG_FULL(format,...)                                             \
do                                                                            \
{                                                                             \
   LOG_FULL (LRU_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,              \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _LRU_CTXT_X
{
   LRU_INIT_PARAMS_X x_init_params;

   HM_HDL hl_hm;
} LRU_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/
LRU_RET_E lru_create_resources (
   LRU_CTXT_X *px_ctxt,
   LRU_INIT_PARAMS_X *px_init_params);

LRU_RET_E lru_destroy_resources (
   LRU_CTXT_X *px_ctxt);

#ifdef   __cplusplus
}
#endif

#endif /* __CH_UTILS_LRU_PRIVATE_H__ */
