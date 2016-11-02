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
 * \file   exp_lru.h
 *
 * \author sandeepprakash
 *
 * \date   20-Oct-2016
 *
 * \brief  Public API of a Least Recently Used (LRU) cache implementation.
 *
 ******************************************************************************/


#ifndef __EXP_LRU_H__
#define __EXP_LRU_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LRU_SIZE_UNLIMITED               (PAL_UINT32_MAX_VALUE)

/******************************** ENUMERATIONS ********************************/
typedef enum _LRU_RET_E
{
   eLRU_RET_SUCCESS             = 0x00000000,

   eLRU_RET_FAILURE,

   eLRU_RET_INVALID_ARGS,

   eLRU_RET_INVALID_HANDLE,

   eLRU_RET_RESOURCE_FAILURE,

   eLRU_RET_EMPTY,

   eLRU_RET_FULL,

   eLRU_RET_MAX
} LRU_RET_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct LRU_CTXT_X       *LRU_HDL;

typedef struct _LRU_INIT_PARAMS_X
{
   uint32_t ui_max_size_bytes;
} LRU_INIT_PARAMS_X;

typedef HM_NODE_DATA_X LRU_NODE_DATA_X;

/***************************** FUNCTION PROTOTYPES ****************************/
LRU_RET_E lru_init (
   LRU_HDL *phl_lru_hdl,
   LRU_INIT_PARAMS_X *px_init_params);

LRU_RET_E lru_deinit (
   LRU_HDL hl_lru_hdl);

LRU_RET_E lru_set (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry);

LRU_RET_E lru_has (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry);

LRU_RET_E lru_get (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry);

LRU_RET_E lru_delete (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_LRU_H__ */
