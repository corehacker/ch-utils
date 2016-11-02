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
   int i_command = -1;
   int i_ret = -1;
   bool b_exit = false;

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




   while (1)
   {
      printf ("Commands:\n\t1. Add\n\t2. Search\n\t3. Delete"
               "\n\t4. Delete All\n\t5. Exit\n"
               "Enter Command: ");
      fflush (stdout);

      PAL_GET_INT32_FROM_CONSOLE(&i_command);

      if (4 != i_command && 5 != i_command && 6 != i_command)
      {
         printf ("Enter String: "); fflush (stdout);

         PAL_GET_STRING_FROM_CONSOLE(uca_str_buf, sizeof(uca_str_buf));
      }

      switch (i_command)
      {
         case 1:
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            x_node_data.x_hm_node.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.x_hm_node.u_hm_key.puc_str_key = uca_str_buf;
            x_node_data.x_hm_node.p_data = malloc (100);
            x_node_data.x_hm_node.ui_data_size = 100;
            e_lru_ret = lru_set (hl_lru_hdl, &x_node_data);
            if (eLRU_RET_SUCCESS != e_lru_ret) {
               LRU_TEST_LOG ("lru_set failed: %d, %p", e_lru_ret, hl_lru_hdl);
            }
            else {
               LRU_TEST_LOG ("lru_set success: %d, %p", e_lru_ret, hl_lru_hdl);
               lru_print (hl_lru_hdl);
            }
            break;
         }
         case 2:
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            x_node_data.x_hm_node.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.x_hm_node.u_hm_key.puc_str_key = uca_str_buf;
            e_lru_ret = lru_has (hl_lru_hdl, &x_node_data);
            if (eLRU_RET_NODE_FOUND == e_lru_ret)
            {
               LRU_TEST_LOG("Key \"%s\" Found: %p", uca_str_buf, x_node_data.x_hm_node.p_data);
            }
            else
            {
               LRU_TEST_LOG("Key \"%s\" Not Found: %d", uca_str_buf, e_lru_ret);
            }
            break;
         }
         case 3:
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            x_node_data.x_hm_node.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.x_hm_node.u_hm_key.puc_str_key = uca_str_buf;
            e_lru_ret = lru_delete (hl_lru_hdl, &x_node_data);
            if (eLRU_RET_SUCCESS == e_lru_ret)
            {
               LRU_TEST_LOG("Key \"%s\" Deleted", uca_str_buf);
            }
            else
            {
               LRU_TEST_LOG("Key \"%s\" Not Found: %d", uca_str_buf, e_lru_ret);
            }

            lru_print (hl_lru_hdl);
            break;
         }
         case 4:
         {
            e_lru_ret = lru_delete_all (hl_lru_hdl);
            lru_print (hl_lru_hdl);
          break;
         }
         case 5:
         {
            b_exit = true;
            break;
         }
      }
      if (true == b_exit)
      {
         break;
      }
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
