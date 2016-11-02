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
 * \file   lru_private.c
 *
 * \author corehacker
 *
 * \date   Oct 22, 2016
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
LRU_RET_E lru_create_resources (
   LRU_CTXT_X *px_ctxt,
   LRU_INIT_PARAMS_X *px_init_params)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   HM_INIT_PARAMS_X x_hm_init_params = {0};

   pal_memmove(&(px_ctxt->x_init_params), px_init_params,
               sizeof (px_ctxt->x_init_params));

   x_hm_init_params.b_maintain_linked_list = true;
   x_hm_init_params.b_thread_safe = false;
   x_hm_init_params.ui_hm_table_size = (1024);
   x_hm_init_params.e_hm_key_type = eHM_KEY_TYPE_STRING;
   x_hm_init_params.ui_linked_list_flags |= eHM_LINKED_LIST_FLAGS_BM_UNSORTED;
   x_hm_init_params.ui_thread_safe_flags |= eHM_THREAD_SAFE_FLAGS_BM_MAP_LEVEL;
   e_hm_ret = hm_create(&(px_ctxt->hl_hm), &x_hm_init_params);
   if (eHM_RET_SUCCESS != e_hm_ret)
   {
      LRU_LOG_LOW("hm_create failed: %d", e_hm_ret);
      e_ret = eLRU_RET_RESOURCE_FAILURE;
   }
   else
   {
      LRU_LOG_LOW("hm_create success: %d", e_hm_ret);
      e_ret = eLRU_RET_SUCCESS;
   }
   return e_ret;
}

LRU_RET_E lru_destroy_resources (
   LRU_CTXT_X *px_ctxt)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;

   if (px_ctxt)
   {
      if (NULL != px_ctxt->hl_hm) {
         (void) hm_delete(px_ctxt->hl_hm);
      }
   }

   e_ret = eLRU_RET_SUCCESS;
   return e_ret;
}
