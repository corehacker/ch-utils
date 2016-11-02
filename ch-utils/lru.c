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
 * \file   lru.c
 *
 * \author corehacker
 *
 * \date   Oct 21, 2016
 *
 * \brief  
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_hashmap.h>
#include <ch-utils/exp_lru.h>
#include "lru_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
LRU_RET_E lru_create (
   LRU_HDL *phl_lru_hdl,
   LRU_INIT_PARAMS_X *px_init_params)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;
   LRU_CTXT_X *px_ctxt = NULL;

   if ((NULL == phl_lru_hdl) || (NULL == px_init_params))
   {
      LRU_LOG_LOW("Invalid arguments: %p", px_init_params);
      e_ret = eLRU_RET_INVALID_ARGS;
   }
   else
   {
      px_ctxt = (LRU_CTXT_X *) pal_malloc (sizeof (LRU_CTXT_X), NULL);
      if (NULL == px_ctxt)
      {
         e_ret = eLRU_RET_RESOURCE_FAILURE;
      }
      else {
         e_ret = lru_create_resources (px_ctxt, px_init_params);
         if (eLRU_RET_SUCCESS == e_ret) {
            *phl_lru_hdl = (LRU_HDL) px_ctxt;
         }
         else {
            LRU_LOG_LOW("lru_create_resources failed: %d", e_ret);
         }
      }
   }
   return e_ret;
}

LRU_RET_E lru_delete (
   LRU_HDL hl_lru_hdl)
{
   LRU_CTXT_X *px_ctxt = NULL;

   px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;

   lru_destroy_resources(px_ctxt);
   return eLRU_RET_SUCCESS;
}
