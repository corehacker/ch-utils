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
 * \file   lru_test.c
 *
 * \author corehacker
 *
 * \date   Oct 25, 2016
 *
 * \brief  
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_hashmap.h>
#include <ch-utils/exp_lru.h>

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LRU_TEST_LOG(format,...)                                            \
   do                                                                      \
   {                                                                       \
      printf("%s: %d: "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);    \
   } while (0)

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
int main ()
{
   int32_t i_ret_val = -1;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   bool b_pal_init = false;
   PAL_LOGGER_INIT_PARAMS_X x_logger_param = { false };
   LRU_RET_E e_lru_ret = eLRU_RET_FAILURE;
   LRU_HDL hl_lru_hdl = NULL;
   LRU_INIT_PARAMS_X x_lru_init_params = {0};
   LRU_NODE_DATA_X x_node_data = {eHM_KEY_TYPE_INVALID};
   uint8_t uca_str_buf[1024] = {0};

   x_logger_param.b_enable_console_logging = true;
   x_logger_param.e_level = eLOG_LEVEL_HIGH;
   pal_logger_env_init (&x_logger_param);

   e_pal_ret = pal_env_init ();
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      LRU_TEST_LOG ("pal_env_init failed: %d", e_pal_ret);
      goto CLEAN_RETURN;
   }
   b_pal_init = true;
   LRU_TEST_LOG ("pal_env_init success");

   x_lru_init_params.ui_max_size_bytes = (1 * 1024 * 1024);
   e_lru_ret = lru_init(&hl_lru_hdl, &x_lru_init_params);
   if ((eLRU_RET_SUCCESS != e_lru_ret) || (NULL == hl_lru_hdl)) {
      LRU_TEST_LOG ("lru_create failed: %d, %p", e_lru_ret, hl_lru_hdl);
      goto CLEAN_RETURN;
   }
   LRU_TEST_LOG ("lru_create success: %d, %p", e_lru_ret, hl_lru_hdl);

   (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
   x_node_data.e_hm_key_type = eHM_KEY_TYPE_STRING;
   x_node_data.u_hm_key.puc_str_key = "test key";
   x_node_data.p_data = malloc (100);
   x_node_data.ui_data_size = 100;
   e_lru_ret = lru_set(hl_lru_hdl, &x_node_data);
   if (eLRU_RET_SUCCESS != e_lru_ret) {
      LRU_TEST_LOG ("lru_set failed: %d, %p", e_lru_ret, hl_lru_hdl);
   }
   else {
      LRU_TEST_LOG ("lru_set success: %d, %p", e_lru_ret, hl_lru_hdl);
      lru_print (hl_lru_hdl);
   }

CLEAN_RETURN:
   if (NULL != hl_lru_hdl) {
      lru_deinit(hl_lru_hdl);
   }
   if (true == b_pal_init)
   {
      e_pal_ret = pal_env_deinit();
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         LRU_TEST_LOG("pal_env_deinit failed: %d", e_pal_ret);
      }
      else
      {
         LRU_TEST_LOG("pal_env_deinit success");
      }
   }
   return i_ret_val;
}
