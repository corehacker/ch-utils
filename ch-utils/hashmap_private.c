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
 * Copyright (c) 2013, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   hashmap_private.c
 *
 * \author sandeepprakash
 *
 * \date   Mar 22, 2013
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_hashmap.h>
#include "hashmap_private.h"
#include "hashmap_private_cbks.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/
static uint32_t jen_hash(
   uint8_t *k,
   uint32_t length,
   uint32_t initval);

static HM_RET_E hm_gen_hash_from_buf (
   HM_CTXT_X *px_hm_ctxt,
   uint8_t *puc_buf,
   uint32_t ui_buf_size,
   uint32_t *pui_hash);

static HM_RET_E hm_get_hash_from_node_data (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data,
   uint32_t *pui_hash);

static HM_TABLE_ENTRY_X *hm_get_new_table_entry_from_hash (
   HM_CTXT_X *px_hm_ctxt,
   uint32_t ui_hash);

/****************************** LOCAL FUNCTIONS *******************************/
/*
 * http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
 */
#define hashsize(n) ( 1U << (n) )
#define hashmask(n) ( hashsize ( n ) - 1 )

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= ( c >> 13 ); \
  b -= c; b -= a; b ^= ( a << 8 ); \
  c -= a; c -= b; c ^= ( b >> 13 ); \
  a -= b; a -= c; a ^= ( c >> 12 ); \
  b -= c; b -= a; b ^= ( a << 16 ); \
  c -= a; c -= b; c ^= ( b >> 5 ); \
  a -= b; a -= c; a ^= ( c >> 3 ); \
  b -= c; b -= a; b ^= ( a << 10 ); \
  c -= a; c -= b; c ^= ( b >> 15 ); \
}

static uint32_t jen_hash(
   uint8_t *k,
   uint32_t length,
   uint32_t initval)
{
   uint32_t a, b;
   uint32_t c = initval;
   uint32_t len = length;

   a = b = 0x9e3779b9;

   /*---------------------------------------- handle most of the key */
   while (len >= 12)
   {
      a += (k [0] + ((uint32_t) k [1] << 8) + ((uint32_t) k [2] << 16)
         + ((uint32_t) k [3] << 24));
      b += (k [4] + ((uint32_t) k [5] << 8) + ((uint32_t) k [6] << 16)
         + ((uint32_t) k [7] << 24));
      c += (k [8] + ((uint32_t) k [9] << 8) + ((uint32_t) k [10] << 16)
         + ((uint32_t) k [11] << 24));

      mix( a, b, c);

      k += 12;
      len -= 12;
   }

   /*------------------------------------- handle the last 11 bytes */
   c += length;
   switch (len) /* all the case statements fall through */
   {
      case 11:
         c += ((uint32_t) k [10] << 24);
      case 10:
         c += ((uint32_t) k [9] << 16);
      case 9:
         c += ((uint32_t) k [8] << 8);
         /* First byte of c reserved for length */
      case 8:
         b += ((uint32_t) k [7] << 24);
      case 7:
         b += ((uint32_t) k [6] << 16);
      case 6:
         b += ((uint32_t) k [5] << 8);
      case 5:
         b += k [4];
      case 4:
         a += ((uint32_t) k [3] << 24);
      case 3:
         a += ((uint32_t) k [2] << 16);
      case 2:
         a += ((uint32_t) k [1] << 8);
      case 1:
         a += k [0];
   }

   mix( a, b, c);
   /*-------------------------------------------- report the result */
   return c;
}

uint32_t djb_hash(
   uint8_t *key,
   int len)
{
   uint8_t *p = key;
   uint32_t h = 0;
   int i;

   for (i = 0; i < len; i++)
      h = 33 * h + p [i];

   return h;
}

static HM_RET_E hm_gen_hash_from_buf (
   HM_CTXT_X *px_hm_ctxt,
   uint8_t *puc_buf,
   uint32_t ui_buf_size,
   uint32_t *pui_hash)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   uint32_t ui_hash = 0;

   if ((NULL == px_hm_ctxt) || (NULL == puc_buf) || (NULL == pui_hash))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   ui_hash = jen_hash (puc_buf, ui_buf_size, 0);

   ui_hash = ui_hash % px_hm_ctxt->x_init_params.ui_hm_table_size;
   *pui_hash = ui_hash;
   e_error = eHM_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_create_resources (
   HM_CTXT_X *px_hm_ctxt)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   uint32_t ui_alloc_size = 0;
   PAL_MUTEX_CREATE_PARAM_X x_mutex_param = {{0}};
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_INIT_PARAMS_X x_list_params = {0};

   if (NULL == px_hm_ctxt)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   ui_alloc_size = px_hm_ctxt->x_init_params.ui_hm_table_size
      * sizeof(HM_TABLE_ENTRY_X *);
   px_hm_ctxt->ppx_hash_table = pal_malloc (ui_alloc_size, NULL );
   if (NULL == px_hm_ctxt->ppx_hash_table)
   {
      HM_LOG_MED("pal_malloc failed");
      e_error = eHM_RET_RESOURCE_FAILURE;
      goto CLEAN_RETURN;
   }

   if ((true == px_hm_ctxt->x_init_params.b_thread_safe)
      && (eHM_THREAD_SAFE_FLAGS_BM_MAP_LEVEL
         & px_hm_ctxt->x_init_params.ui_thread_safe_flags))
   {
      e_pal_ret = pal_mutex_create (&(px_hm_ctxt->hl_mutex_hdl),
         &x_mutex_param);
      if ((ePAL_RET_SUCCESS != e_pal_ret) || (NULL == px_hm_ctxt->hl_mutex_hdl))
      {
         HM_LOG_MED("pal_mutex_create failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      else
      {
         e_error = eHM_RET_SUCCESS;
      }
   }
   else
   {
      e_error = eHM_RET_SUCCESS;
   }

   if (true == px_hm_ctxt->x_init_params.b_maintain_linked_list)
   {
      px_hm_ctxt->px_link_list_ctxt = pal_malloc (sizeof(HM_LINK_LIST_CTXT_X),
         NULL);
      if (NULL == px_hm_ctxt->px_link_list_ctxt)
      {
         HM_LOG_MED("pal_malloc failed");
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }

      x_list_params.ui_list_max_elements = LIST_SIZE_UNLIMITED;
      e_list_ret = list_create(&(px_hm_ctxt->px_link_list_ctxt->hl_list_hdl),
         &x_list_params);
      if ((eLIST_RET_SUCCESS != e_list_ret)
         || (NULL == px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
      {
         HM_LOG_MED("list_create failed: %d", e_list_ret,
            px_hm_ctxt->px_link_list_ctxt->hl_list_hdl);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      else
      {
         e_error = eHM_RET_SUCCESS;
      }
   }
   else
   {
      e_error = eHM_RET_SUCCESS;
   }
CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_delete_resources (
   HM_CTXT_X *px_hm_ctxt)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;

   if (NULL == px_hm_ctxt)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_destroy (px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_destroy failed: %d", e_pal_ret);
      }
      px_hm_ctxt->hl_mutex_hdl = NULL;
   }

   if (NULL != px_hm_ctxt->ppx_hash_table)
   {
      pal_free (px_hm_ctxt->ppx_hash_table);
      px_hm_ctxt->ppx_hash_table = NULL;
   }

   if (NULL != px_hm_ctxt->px_link_list_ctxt)
   {
      if (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl)
      {
         e_list_ret = list_delete (px_hm_ctxt->px_link_list_ctxt->hl_list_hdl);
         if (eLIST_RET_SUCCESS != e_list_ret)
         {
            HM_LOG_MED("list_delete failed: %d", e_list_ret);
         }
         px_hm_ctxt->px_link_list_ctxt->hl_list_hdl = NULL;
      }
      pal_free (px_hm_ctxt->px_link_list_ctxt);
      px_hm_ctxt->px_link_list_ctxt = NULL;
   }

   e_error = eHM_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

static HM_RET_E hm_get_hash_from_node_data (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data,
   uint32_t *pui_hash)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   uint32_t ui_key_len = 0;
   uint8_t *puc_key_buf = NULL;

   if ((NULL == px_hm_ctxt) || (NULL == px_node_data) || (NULL == pui_hash))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (px_hm_ctxt->x_init_params.e_hm_key_type != px_node_data->e_hm_key_type)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   switch (px_node_data->e_hm_key_type)
   {
      case eHM_KEY_TYPE_INT:
      {
         ui_key_len = sizeof(px_node_data->u_hm_key.ui_uint_key);
         puc_key_buf = (uint8_t *) &(px_node_data->u_hm_key.ui_uint_key);
         break;
      }
      case eHM_KEY_TYPE_STRING:
      {
         ui_key_len = pal_strlen (px_node_data->u_hm_key.puc_str_key);
         puc_key_buf = px_node_data->u_hm_key.puc_str_key;
         break;
      }
      default:
      {
         break;
      }
   }
   e_error = hm_gen_hash_from_buf (px_hm_ctxt, puc_key_buf, ui_key_len,
      pui_hash);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hashmap_gen_hash_from_buf failed: %d", e_error);
   }
CLEAN_RETURN:
   return e_error;
}

static HM_TABLE_ENTRY_X *hm_get_new_table_entry_from_hash (
   HM_CTXT_X *px_hm_ctxt,
   uint32_t ui_hash)
{
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_INIT_PARAMS_X x_init_params = {0};

   if (NULL == px_hm_ctxt)
   {
      HM_LOG_MED("Invalid Args");
      goto CLEAN_RETURN;
   }

   px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_hash];
   if (NULL == px_hm_table_entry)
   {
      HM_LOG_HIGH("Table entry @ %d position needs to be allocated", ui_hash);
      px_hm_table_entry = pal_malloc (sizeof(HM_TABLE_ENTRY_X), NULL);
      if (NULL == px_hm_table_entry)
      {
         HM_LOG_MED("pal_malloc failed");
         goto CLEAN_RETURN;
      }
      px_hm_ctxt->ppx_hash_table[ui_hash] = px_hm_table_entry;
   }

   if (NULL == px_hm_table_entry->hl_list_hdl)
   {
      HM_LOG_HIGH("List @ %d position needs to be allocated", ui_hash);
      x_init_params.ui_list_max_elements = LIST_SIZE_UNLIMITED;
      e_list_ret = list_create (&(px_hm_table_entry->hl_list_hdl),
         &x_init_params);
      if ((eLIST_RET_SUCCESS != e_list_ret)
         || (NULL == px_hm_table_entry->hl_list_hdl))
      {
         HM_LOG_MED("list_create failed: %d",
            e_list_ret, px_hm_table_entry->hl_list_hdl);
         pal_free (px_hm_table_entry);
         px_hm_table_entry = NULL;
      }
   }

CLEAN_RETURN:
   return px_hm_table_entry;
}

HM_TABLE_ENTRY_X *hm_get_new_table_entry (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   uint32_t ui_hash = 0;

   if ((NULL == px_hm_ctxt) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      px_hm_table_entry = NULL;
      goto CLEAN_RETURN;
   }

   e_error = hm_get_hash_from_node_data (px_hm_ctxt, px_node_data, &ui_hash);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hashmap_gen_hash_from_buf failed: %d", e_error);
      px_hm_table_entry = NULL;
      goto CLEAN_RETURN;
   }
   HM_LOG_HIGH("Hash Value: %d", ui_hash);

   px_hm_table_entry = hm_get_new_table_entry_from_hash (px_hm_ctxt, ui_hash);
   if (NULL == px_hm_table_entry)
   {
      HM_LOG_MED("hm_get_table_entry_from_hash failed");
      px_hm_table_entry = NULL;
   }
CLEAN_RETURN:
   return px_hm_table_entry;
}

HM_TABLE_ENTRY_X *hm_get_table_entry (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   uint32_t ui_hash = 0;

   if ((NULL == px_hm_ctxt) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      px_hm_table_entry = NULL;
      goto CLEAN_RETURN;
   }

   e_error = hm_get_hash_from_node_data (px_hm_ctxt, px_node_data, &ui_hash);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hashmap_gen_hash_from_buf failed: %d", e_error);
      px_hm_table_entry = NULL;
      goto CLEAN_RETURN;
   }
   HM_LOG_HIGH("Hash Value: %d", ui_hash);
   px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_hash];
CLEAN_RETURN:
   return px_hm_table_entry;
}

HM_RET_E hm_delete_table_entry_from_hash (
   HM_CTXT_X *px_hm_ctxt,
   HM_NODE_DATA_X *px_node_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   uint32_t ui_hash = 0;

   if ((NULL == px_hm_ctxt) || (NULL == px_node_data))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   e_error = hm_get_hash_from_node_data (px_hm_ctxt, px_node_data, &ui_hash);
   if (eHM_RET_SUCCESS != e_error)
   {
      HM_LOG_MED("hashmap_gen_hash_from_buf failed: %d", e_error);
      goto CLEAN_RETURN;
   }
   HM_LOG_MED("Hash Value: %d", ui_hash);

   px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_hash];
   if (NULL != px_hm_table_entry)
   {
      if (NULL != px_hm_table_entry->hl_list_hdl)
      {
         e_list_ret = list_delete (px_hm_table_entry->hl_list_hdl);
         if (eLIST_RET_SUCCESS != e_list_ret)
         {
            HM_LOG_MED("list_delete failed: %d", e_list_ret);
         }
         px_hm_table_entry->hl_list_hdl = NULL;
      }
      pal_free (px_hm_table_entry);
      px_hm_table_entry = NULL;

      px_hm_ctxt->ppx_hash_table[ui_hash] = NULL;
   }
   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

HM_NODE_DATA_X *hm_alloc_node (
   HM_NODE_DATA_X *px_node_data)
{
   HM_NODE_DATA_X *px_new_node = NULL;
   uint32_t ui_key_len = 0;

   if (NULL == px_node_data)
   {
      HM_LOG_MED("Invalid Args");
      goto CLEAN_RETURN;
   }

   px_new_node = pal_malloc (sizeof(HM_NODE_DATA_X), NULL);
   if (NULL == px_new_node)
   {
      HM_LOG_MED("pal_malloc failed");
      goto CLEAN_RETURN;
   }

   (void) pal_memmove(px_new_node, px_node_data, sizeof (*px_new_node));

   if (eHM_KEY_TYPE_STRING == px_node_data->e_hm_key_type)
   {
      ui_key_len = pal_strlen (px_node_data->u_hm_key.puc_str_key);
      ui_key_len = ui_key_len + 1;
      px_new_node->u_hm_key.puc_str_key = pal_malloc (ui_key_len, NULL);
      if (NULL == px_new_node->u_hm_key.puc_str_key)
      {
         pal_free (px_new_node);
         px_new_node = NULL;
         goto CLEAN_RETURN;
      }

      (void) pal_strncpy (px_new_node->u_hm_key.puc_str_key,
         px_node_data->u_hm_key.puc_str_key, ui_key_len);
   }
CLEAN_RETURN:
   return px_new_node;
}


void hm_dealloc_node (
   HM_NODE_DATA_X *px_node_data)
{
   if (NULL != px_node_data)
   {
      if (eHM_KEY_TYPE_STRING == px_node_data->e_hm_key_type)
      {
         if (NULL != px_node_data->u_hm_key.puc_str_key)
         {
            pal_free (px_node_data->u_hm_key.puc_str_key);
            px_node_data->u_hm_key.puc_str_key = NULL;
         }
      }
      pal_free(px_node_data);
      px_node_data = NULL;
   }
}

HM_RET_E hm_for_each_hm_forward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   uint32_t ui_i = 0;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   uint32_t ui_list_len = 0;
   HM_FOR_EACH_METADATA_X x_metadata = {NULL};
   bool b_locked = false;

   if ((NULL == px_hm_ctxt) || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   for (ui_i = 0; ui_i < px_hm_ctxt->x_init_params.ui_hm_table_size; ui_i++)
   {
      px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_i];
      if (NULL == px_hm_table_entry)
      {
         continue;
      }
      if (NULL == px_hm_table_entry->hl_list_hdl)
      {
         continue;
      }
      e_list_ret = list_get_len (px_hm_table_entry->hl_list_hdl,
         &ui_list_len);
      if ((eLIST_RET_SUCCESS != e_list_ret) || (0 == ui_list_len))
      {
         continue;
      }

      x_metadata.px_hm_ctxt = px_hm_ctxt;
      x_metadata.fn_hm_for_each_cbk = fn_for_each_cbk;
      x_metadata.p_hm_for_each_app_data = p_app_data;
      e_list_ret = list_for_all_nodes (px_hm_table_entry->hl_list_hdl,
         hm_list_for_all_cbk, &x_metadata);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         HM_LOG_MED("list_for_all_nodes failed: %d", e_list_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         break;
      }
      else
      {
         e_error = eHM_RET_SUCCESS;
      }
   }

CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_for_each_hm_backward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_NOT_IMPLEMENTED;
   return e_error;
}

HM_RET_E hm_for_each_linked_list_forward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   bool b_locked = false;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   HM_FOR_EACH_METADATA_X x_metadata = {NULL};

   if ((NULL == px_hm_ctxt) || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   if ((NULL != px_hm_ctxt->px_link_list_ctxt)
      && (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl))
   {
      x_metadata.px_hm_ctxt = px_hm_ctxt;
      x_metadata.fn_hm_for_each_cbk = fn_for_each_cbk;
      x_metadata.p_hm_for_each_app_data = p_app_data;
      e_list_ret = list_for_all_nodes (
         px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, hm_list_for_all_cbk,
         &x_metadata);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         HM_LOG_MED("list_for_all_nodes failed: %d", e_list_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
      else
      {
         e_error = eHM_RET_SUCCESS;
      }
   }
   else
   {
      e_error = eHM_RET_SUCCESS;
   }
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}

HM_RET_E hm_for_each_linked_list_backward (
   HM_CTXT_X *px_hm_ctxt,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_NOT_IMPLEMENTED;
   return e_error;
}

HM_RET_E hm_for_each_hm (
   HM_CTXT_X *px_hm_ctxt,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;

   if ((NULL == px_hm_ctxt) || (NULL == px_for_each_param)
      || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   switch (px_for_each_param->e_direction)
   {
      case eHM_FOR_EACH_DIRECTION_FORWARD:
      {
         e_error = hm_for_each_hm_forward (px_hm_ctxt, fn_for_each_cbk,
            p_app_data);
         break;
      }
      case eHM_FOR_EACH_DIRECTION_BACKWARD:
      {
         e_error = hm_for_each_hm_backward (px_hm_ctxt, fn_for_each_cbk,
            p_app_data);
         break;
      }
      default:
      {
         HM_LOG_MED("Invalid Args");
         e_error = eLIST_RET_INVALID_ARGS;
         break;
      }
   }

CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_for_each_linked_list (
   HM_CTXT_X *px_hm_ctxt,
   HM_FOR_EACH_PARAMS_X *px_for_each_param,
   pfn_hm_for_each_cbk fn_for_each_cbk,
   void *p_app_data)
{
   HM_RET_E e_error = eHM_RET_FAILURE;

   if ((NULL == px_hm_ctxt) || (NULL == px_for_each_param)
      || (NULL == fn_for_each_cbk))
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   switch (px_for_each_param->e_direction)
   {
      case eHM_FOR_EACH_DIRECTION_FORWARD:
      {
         e_error = hm_for_each_linked_list_forward (px_hm_ctxt, fn_for_each_cbk,
            p_app_data);
         break;
      }
      case eHM_FOR_EACH_DIRECTION_BACKWARD:
      {
         e_error = hm_for_each_linked_list_backward (px_hm_ctxt,
            fn_for_each_cbk, p_app_data);
         break;
      }
      default:
      {
         HM_LOG_MED("Invalid Args");
         e_error = eLIST_RET_INVALID_ARGS;
         break;
      }
   }

CLEAN_RETURN:
   return e_error;
}

HM_RET_E hm_delete_all_nodes_pvt (
   HM_CTXT_X *px_hm_ctxt)
{
   HM_RET_E e_error = eHM_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   bool b_locked = false;
   HM_TABLE_ENTRY_X *px_hm_table_entry = NULL;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   uint32_t ui_list_len = 0;
   uint32_t ui_i = 0;
   LIST_NODE_DATA_X x_node_data = {NULL};
   HM_NODE_DATA_X *px_node_data = NULL;

   if (NULL == px_hm_ctxt)
   {
      HM_LOG_MED("Invalid Args");
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (NULL != px_hm_ctxt->hl_mutex_hdl)
   {
      e_pal_ret = pal_mutex_lock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_lock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
         goto CLEAN_RETURN;
      }
      b_locked = true;
   }

   for (ui_i = 0; ui_i < px_hm_ctxt->x_init_params.ui_hm_table_size; ui_i++)
   {
      px_hm_table_entry = px_hm_ctxt->ppx_hash_table[ui_i];
      if (NULL == px_hm_table_entry)
      {
         continue;
      }
      if (NULL != px_hm_table_entry->hl_list_hdl)
      {
         do
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            e_list_ret = list_node_delete_at_head (
               px_hm_table_entry->hl_list_hdl, &x_node_data);
            if (NULL != x_node_data.p_data)
            {
               px_node_data = (HM_NODE_DATA_X *) x_node_data.p_data;
               hm_dealloc_node (px_node_data);
               px_node_data = NULL;
            }

            e_list_ret = list_node_peek_at_head (px_hm_table_entry->hl_list_hdl,
               &x_node_data);
         } while (eLIST_RET_LIST_EMPTY != e_list_ret);

         e_list_ret = list_delete (px_hm_table_entry->hl_list_hdl);
         if (eLIST_RET_SUCCESS != e_list_ret)
         {
            HM_LOG_MED("list_delete failed: %d", e_list_ret);
         }
         px_hm_table_entry->hl_list_hdl = NULL;
      }

      pal_free (px_hm_table_entry);
      px_hm_table_entry = NULL;

      px_hm_ctxt->ppx_hash_table[ui_i] = NULL;
   }

   if (NULL != px_hm_ctxt->px_link_list_ctxt)
   {
      if (NULL != px_hm_ctxt->px_link_list_ctxt->hl_list_hdl)
      {
         do
         {
            e_list_ret = list_node_delete_at_head (
               px_hm_ctxt->px_link_list_ctxt->hl_list_hdl, &x_node_data);
         } while (eLIST_RET_LIST_EMPTY != e_list_ret);
      }
   }
   e_error = eHM_RET_SUCCESS;
CLEAN_RETURN:
   if ((true == b_locked) && (NULL != px_hm_ctxt->hl_mutex_hdl))
   {
      e_pal_ret = pal_mutex_unlock(px_hm_ctxt->hl_mutex_hdl);
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_LOG_MED("pal_mutex_unlock failed: %d", e_pal_ret);
         e_error = eHM_RET_RESOURCE_FAILURE;
      }
   }
   return e_error;
}
