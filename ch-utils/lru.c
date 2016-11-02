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
LRU_RET_E lru_init (
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

LRU_RET_E lru_deinit (
   LRU_HDL hl_lru_hdl)
{
   LRU_CTXT_X *px_ctxt = NULL;

   px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;

   lru_destroy_resources(px_ctxt);
   return eLRU_RET_SUCCESS;
}

LRU_RET_E lru_set (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   LRU_CTXT_X *px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;

   if (NULL == hl_lru_hdl || NULL == px_entry) {
      e_ret = eLRU_RET_INVALID_ARGS;
   }
   else {
      px_entry->ui_created = pal_get_system_time_ns ();
      px_entry->ui_accessed = pal_get_system_time_ns ();
      e_hm_ret = hm_add_node (px_ctxt->hl_hm, (HM_NODE_DATA_X *) px_entry);
      if (eHM_RET_SUCCESS != e_hm_ret) {
         e_ret = eLRU_RET_RESOURCE_FAILURE;
      }
      else {
         e_ret = eLRU_RET_SUCCESS;
      }
   }

   return e_ret;
}

LRU_RET_E lru_has (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   LRU_CTXT_X *px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;

   if (NULL == hl_lru_hdl || NULL == px_entry) {
      e_ret = eLRU_RET_INVALID_ARGS;
   }
   else {
      e_hm_ret = hm_search_node (px_ctxt->hl_hm, (HM_NODE_DATA_X *) px_entry);
      if (eHM_RET_HM_NODE_FOUND != e_hm_ret) {
         e_ret = eLRU_RET_NODE_NOT_FOUND;
      }
      else {
         e_ret = eLRU_RET_NODE_FOUND;
      }
   }
   return e_ret;
}

LRU_RET_E lru_get (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;

   return e_ret;
}

LRU_RET_E lru_delete (
   LRU_HDL hl_lru_hdl,
   LRU_NODE_DATA_X *px_entry)
{
   LRU_RET_E e_ret = eLRU_RET_FAILURE;
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   LRU_CTXT_X *px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;

   if (NULL == hl_lru_hdl || NULL == px_entry) {
      e_ret = eLRU_RET_INVALID_ARGS;
   }
   else {
      e_hm_ret = hm_delete_node(px_ctxt->hl_hm, (HM_NODE_DATA_X *) px_entry);
      if (eHM_RET_SUCCESS != e_hm_ret) {
         e_ret = eLRU_RET_RESOURCE_FAILURE;
      }
      else {
         e_ret = eLRU_RET_SUCCESS;
      }
   }

   return e_ret;
}

void lru_print (
   LRU_HDL hl_lru_hdl)
{
   LRU_CTXT_X *px_ctxt = (LRU_CTXT_X *) hl_lru_hdl;
   hm_print_all_nodes (px_ctxt->hl_hm);
}
