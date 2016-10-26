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

   e_hm_ret = hm_create(&(px_ctxt->hl_hm), &x_hm_init_params);
   if (eHM_RET_SUCCESS != e_hm_ret)
   {
      e_ret = eLRU_RET_RESOURCE_FAILURE;
   }
   else
   {

   }
   return e_ret;
}

LRU_RET_E lru_destroy_resources (
   LRU_CTXT_X *px_ctxt)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;

   return e_ret;
}
