/*******************************************************************************
 * Copyright (c) 2013, Sandeep Prakash <123sandy@gmail.com>
 * All rights reserved.
 *
 * \file   hashmap_test.c
 * \author sandeepprakash
 *
 * \date   Mar 21, 2013
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>

#include <ch-utils/exp_hashmap.h>

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define HM_TEST_LOG(format,...)                                            \
   do                                                                      \
   {                                                                       \
      printf("%s: %d: "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);    \
   } while (0)

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
HM_RET_E fn_hm_for_each_cbk (
   HM_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;

   printf ("%s\n", px_curr_node_data->u_hm_key.puc_str_key);
   e_hm_ret = eHM_RET_SUCCESS;
   return e_hm_ret;
}

/*
 * - Return
 *    - eHM_RET_CMP_GREATER -  px_node_a_data > px_node_b_data based on some
 *                               application logic.
 *    - eHM_RET_CMP_LESSER  -  px_node_a_data < px_node_b_data based on some
 *                               application logic.
 *    - eHM_RET_CMP_EQUAL   -  px_node_a_data = px_node_b_data based on some
 *                               application logic.
 */
HM_RET_E fn_hm_compare_cbk (
      HM_NODE_DATA_X *px_node_a_data,
      HM_NODE_DATA_X *px_node_b_data,
      void *p_app_data)
{
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   int i_ret_val = -1;

   if ((NULL == px_node_a_data) || (NULL == px_node_b_data))
   {
      HM_TEST_LOG ("Invalid args");
      e_hm_ret = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((eHM_KEY_TYPE_STRING != px_node_a_data->e_hm_key_type)
      || (eHM_KEY_TYPE_STRING != px_node_b_data->e_hm_key_type))
   {
      HM_TEST_LOG ("Invalid args");
      e_hm_ret = eHM_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   HM_TEST_LOG ("Comparing \"%s\" and \"%s\"",
      px_node_a_data->u_hm_key.puc_str_key,
      px_node_b_data->u_hm_key.puc_str_key);
   i_ret_val = strcmp (px_node_a_data->u_hm_key.puc_str_key,
      px_node_b_data->u_hm_key.puc_str_key);
   if (i_ret_val < 0)
   {
      e_hm_ret = eHM_RET_CMP_GREATER;
   }
   else if (i_ret_val > 0)
   {
      e_hm_ret = eHM_RET_CMP_LESSER;
   }
   else
   {
      e_hm_ret = eHM_RET_CMP_EQUAL;
   }
CLEAN_RETURN:
   return e_hm_ret;
}

HM_RET_E fn_hm_for_each_delete_cbk(
   HM_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;

   if (NULL != px_curr_node_data)
   {
	   if (NULL != px_curr_node_data->p_data)
	   {
		   pal_free (px_curr_node_data->p_data);
		   px_curr_node_data->p_data = NULL;
	   }
   }

   e_hm_ret = eHM_RET_SUCCESS;
LBL_CLEANUP:
   return e_hm_ret;
}

int main ()
{
   int32_t i_ret_val = -1;
   HM_RET_E e_hm_ret = eHM_RET_FAILURE;
   HM_HDL hl_hm_hdl = NULL;
   HM_INIT_PARAMS_X x_hm_params = {0};
   PAL_LOGGER_INIT_PARAMS_X x_logger_param = { false };
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   bool b_pal_init = false;
   HM_NODE_DATA_X x_node_data = {eHM_KEY_TYPE_INVALID};
   uint8_t uca_str_buf[1024] = {0};
   int i_command = -1;
   int i_ret = -1;
   bool b_exit = false;
   HM_FOR_EACH_PARAMS_X x_for_each_param = {eHM_DATA_STRUCT_INVALID};

   x_logger_param.b_enable_console_logging = true;
   x_logger_param.e_level = eLOG_LEVEL_HIGH;
   pal_logger_env_init (&x_logger_param);

   e_pal_ret = pal_env_init ();
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      HM_TEST_LOG ("pal_env_init failed: %d", e_pal_ret);
      goto CLEAN_RETURN;
   }
   b_pal_init = true;
   HM_TEST_LOG ("pal_env_init success");

   x_hm_params.ui_hm_table_size = 10;
   x_hm_params.e_hm_key_type = eHM_KEY_TYPE_STRING;
   x_hm_params.b_maintain_linked_list = true;
   x_hm_params.ui_linked_list_flags |= eHM_LINKED_LIST_FLAGS_BM_SORTED;
   x_hm_params.fn_hm_compare_cbk = fn_hm_compare_cbk;
   e_hm_ret = hm_create (&hl_hm_hdl, &x_hm_params);
   if (eHM_RET_SUCCESS != e_hm_ret)
   {
      HM_TEST_LOG("hm_create failed: %d", e_hm_ret);
      goto CLEAN_RETURN;
   }

   while (1)
   {
      printf ("Commands:\n\t1. Add\n\t2. Search\n\t3. Delete\n\t4. Sort"
    		  "\n\t5. Delete All\n\t6. Exit\n"
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
            x_node_data.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.u_hm_key.puc_str_key = uca_str_buf;
            x_node_data.p_data = malloc (100);
            x_node_data.ui_data_size = 100;
            e_hm_ret = hm_add_node (hl_hm_hdl, &x_node_data);
            if (eHM_RET_SUCCESS != e_hm_ret)
            {
               HM_TEST_LOG("hm_add_node failed: %d", e_hm_ret);
            }
            hm_print_all_nodes (hl_hm_hdl);
            hm_for_each(hl_hm_hdl, fn_hm_for_each_cbk, NULL);

            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS STARTS******************\n");
            x_for_each_param.e_data_structure = eHM_DATA_STRUCT_LINKED_LIST;
            x_for_each_param.e_direction = eHM_FOR_EACH_DIRECTION_FORWARD;
            hm_for_each_v2(hl_hm_hdl, &x_for_each_param, fn_hm_for_each_cbk, NULL);
            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS ENDS******************\n");
            break;
         }
         case 2:
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            x_node_data.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.u_hm_key.puc_str_key = uca_str_buf;
            e_hm_ret = hm_search_node (hl_hm_hdl, &x_node_data);
            if (eHM_RET_HM_NODE_FOUND == e_hm_ret)
            {
               HM_TEST_LOG("Key \"%s\" Found: %p", uca_str_buf, x_node_data.p_data);
            }
            else
            {
               HM_TEST_LOG("Key \"%s\" Not Found: %d", uca_str_buf, e_hm_ret);
            }
            break;
         }
         case 3:
         {
            (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
            x_node_data.e_hm_key_type = eHM_KEY_TYPE_STRING;
            x_node_data.u_hm_key.puc_str_key = uca_str_buf;
            e_hm_ret = hm_delete_node (hl_hm_hdl, &x_node_data);
            if (eHM_RET_SUCCESS == e_hm_ret)
            {
               HM_TEST_LOG("Key \"%s\" Deleted", uca_str_buf);
            }
            else
            {
               HM_TEST_LOG("Key \"%s\" Not Found: %d", uca_str_buf, e_hm_ret);
            }

            hm_print_all_nodes (hl_hm_hdl);
            break;
         }
         case 4:
         {
            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS STARTS - UNSORTED******************\n");
            x_for_each_param.e_data_structure = eHM_DATA_STRUCT_LINKED_LIST;
            x_for_each_param.e_direction = eHM_FOR_EACH_DIRECTION_FORWARD;
            hm_for_each_v2(hl_hm_hdl, &x_for_each_param, fn_hm_for_each_cbk, NULL);
            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS ENDS - UNSORTED******************\n");
            e_hm_ret = hm_linked_list_sort (hl_hm_hdl, fn_hm_compare_cbk, NULL);
            if (eHM_RET_SUCCESS != e_hm_ret)
            {
               HM_TEST_LOG("hm_linked_list_sort failed: %d", e_hm_ret);
            }
            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS STARTS - SORTED******************\n");
            x_for_each_param.e_data_structure = eHM_DATA_STRUCT_LINKED_LIST;
            x_for_each_param.e_direction = eHM_FOR_EACH_DIRECTION_FORWARD;
            hm_for_each_v2(hl_hm_hdl, &x_for_each_param, fn_hm_for_each_cbk, NULL);
            printf (
               "\n\n*******************HASH MAP (LINKED LIST) CONTENTS ENDS - SORTED******************\n");

            break;
         }
         case 5:
         {
        	   x_for_each_param.e_data_structure = eHM_DATA_STRUCT_LINKED_LIST;
        	   x_for_each_param.e_direction = eHM_FOR_EACH_DIRECTION_FORWARD;
        	   e_hm_ret = hm_for_each_v2 (hl_hm_hdl, &x_for_each_param,
        	      fn_hm_for_each_delete_cbk, NULL);

        	   e_hm_ret = hm_delete_all_nodes(hl_hm_hdl);
        	 break;
         }
         case 6:
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

   e_hm_ret = hm_delete (hl_hm_hdl);
   if (eHM_RET_SUCCESS != e_hm_ret)
   {
      HM_TEST_LOG("hm_delete failed: %d", e_hm_ret);
   }
   else
   {
      i_ret_val = 0;
   }
CLEAN_RETURN:
   if (true == b_pal_init)
   {
      e_pal_ret = pal_env_deinit();
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         HM_TEST_LOG("pal_env_deinit failed: %d", e_pal_ret);
      }
      else
      {
         HM_TEST_LOG("pal_env_deinit success");
      }
   }
   return i_ret_val;
}
