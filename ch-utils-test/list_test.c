/*******************************************************************************
 * Copyright (c) 2012, Sandeep Prakash <123sandy@gmail.com>
 * All rights reserved.
 *
 * \file   list_test.c
 * \author sandeepprakash
 *
 * \date   19-Sep-2012
 *
 * \brief  
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>

#include <ch-utils/exp_list.h>
/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define LIST_TEST_LOG(format,...)                                          \
   do                                                                      \
   {                                                                       \
      printf("%s: %d: "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);    \
   } while (0)
/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/
typedef struct _TEST_NODE_DATA_X
{
   uint32_t ui_value;

   uint32_t ui_timestamp;
} TEST_NODE_DATA_X;

/************************ STATIC FUNCTION PROTOTYPES **************************/
static LIST_RET_E fn_for_all_cbk(
   LIST_NODE_DATA_X *px_node_data,
   void *p_app_data);

static LIST_RET_E fn_search_node_cbk (
   LIST_NODE_DATA_X *px_search_node_data,
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

static LIST_RET_E fn_list_compare_fn_cbk (
  LIST_NODE_DATA_X *px_app_node_data,
  LIST_NODE_DATA_X *px_curr_node_data,
  void *p_app_data);

/****************************** LOCAL FUNCTIONS *******************************/
static LIST_RET_E fn_for_all_cbk(
   LIST_NODE_DATA_X *px_node_data,
   void *p_app_data)
{
   LIST_RET_E e_error = eLIST_RET_FAILURE;
   TEST_NODE_DATA_X *px_test_node_data = NULL;

   if (NULL == px_node_data)
   {
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if ((NULL == px_node_data->p_data) || (0 == px_node_data->ui_data_size ||
      (sizeof (TEST_NODE_DATA_X) != px_node_data->ui_data_size)))
   {
      e_error = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_test_node_data = (TEST_NODE_DATA_X *) px_node_data->p_data;

   printf ("%d,%d --> ", px_test_node_data->ui_value,
      px_test_node_data->ui_timestamp);

   e_error = eLIST_RET_SUCCESS;
CLEAN_RETURN:
   return e_error;
}

static LIST_RET_E fn_search_node_cbk (
   LIST_NODE_DATA_X *px_search_node_data,
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   TEST_NODE_DATA_X *px_test_node_data = NULL;
   TEST_NODE_DATA_X *px_test_search_node_data = NULL;

   if ((NULL == px_curr_node_data) || (NULL == px_search_node_data))
   {
      LIST_TEST_LOG("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_test_node_data = (TEST_NODE_DATA_X *) px_curr_node_data->p_data;
   px_test_search_node_data = (TEST_NODE_DATA_X *) px_search_node_data->p_data;

   if (px_test_node_data->ui_value == px_test_search_node_data->ui_value)
   {
      e_list_ret = eLIST_RET_LIST_NODE_FOUND;
   }
   else
   {
      e_list_ret = eLIST_RET_LIST_NODE_NOT_FOUND;
   }
CLEAN_RETURN:
   return e_list_ret;
}

static LIST_RET_E fn_list_compare_fn_cbk (
  LIST_NODE_DATA_X *px_node_a_data,
  LIST_NODE_DATA_X *px_node_b_data,
  void *p_app_data)
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   TEST_NODE_DATA_X *px_test_node_a_data = NULL;
   TEST_NODE_DATA_X *px_test_node_b_data = NULL;

   if ((NULL == px_node_b_data) || (NULL == px_node_a_data))
   {
      LIST_TEST_LOG("Invalid Args");
      e_list_ret = eLIST_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   px_test_node_a_data = (TEST_NODE_DATA_X *) px_node_a_data->p_data;
   px_test_node_b_data = (TEST_NODE_DATA_X *) px_node_b_data->p_data;

   LIST_TEST_LOG("Comparing %d, %d", px_test_node_a_data->ui_value,
      px_test_node_b_data->ui_value);

   if (px_test_node_a_data->ui_value < px_test_node_b_data->ui_value)
   {
      e_list_ret = eLIST_RET_CMP_LESSER;
   }
   else if (px_test_node_a_data->ui_value > px_test_node_b_data->ui_value)
   {
      e_list_ret = eLIST_RET_CMP_GREATER;
   }
   else
   {
      e_list_ret = eLIST_RET_CMP_EQUAL;
   }

CLEAN_RETURN:
   return e_list_ret;
}

int test_case_list_sorted_merge ()
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_INIT_PARAMS_X x_init_params = {0};
   LIST_HDL hl_list1_hdl = NULL;
   LIST_HDL hl_list2_hdl = NULL;
   uint32_t uia_array1[] = {};
   uint32_t uia_array2[] = {1,2,3,4,5,6,7,8,9};
   uint32_t ui_n1 = sizeof(uia_array1) / sizeof(uint32_t);
   uint32_t ui_n2 = sizeof(uia_array2) / sizeof(uint32_t);
   uint32_t ui_i = 0;
   int i_error = -1;
   TEST_NODE_DATA_X x_node_data = {0};
   TEST_NODE_DATA_X *px_node_data = NULL;
   LIST_NODE_DATA_X x_list_node_data = {0};

   x_init_params.ui_list_max_elements = 10;
   e_list_ret = list_create(&hl_list1_hdl, &x_init_params);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (NULL == hl_list1_hdl))
   {
      LIST_TEST_LOG("list_create failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   LIST_TEST_LOG("list_create success");

   x_init_params.ui_list_max_elements = 10;
   e_list_ret = list_create(&hl_list2_hdl, &x_init_params);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (NULL == hl_list2_hdl))
   {
      LIST_TEST_LOG("list_create failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   LIST_TEST_LOG("list_create success");

   while (ui_i < ui_n1)
   {
      (void) pal_memset (&x_node_data, 0x00, sizeof(x_node_data));
      x_node_data.ui_timestamp = pal_get_system_time ();
      x_node_data.ui_value = (uint32_t) uia_array1[ui_i];
      px_node_data = pal_malloc (sizeof(TEST_NODE_DATA_X), NULL );
      if (NULL != px_node_data)
      {
         (void) pal_memmove (px_node_data, &x_node_data,
            sizeof(*px_node_data));
         x_list_node_data.p_data = px_node_data;
         x_list_node_data.ui_data_size = sizeof(*px_node_data);
      }

      e_list_ret = list_node_append (hl_list1_hdl, &x_list_node_data);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         if (eLIST_RET_LIST_FULL == e_list_ret)
         {
            LIST_TEST_LOG("List Full");
            i_error = 0;
         }
         else
         {
            LIST_TEST_LOG("list_node_append failed: %d", e_list_ret);
         }

      }

      ui_i++;
   }

   printf ("\n\nSorted List1 contents: \n***************************************\n");
   list_print_all_nodes (hl_list1_hdl);
   e_list_ret = list_for_all_nodes (hl_list1_hdl, fn_for_all_cbk, NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
      }
   }
   printf ("\n***************************************\n");

   ui_i = 0;
   while (ui_i < ui_n2)
   {
      x_node_data.ui_timestamp = pal_get_system_time ();
      x_node_data.ui_value = (uint32_t) uia_array2[ui_i];
      px_node_data = pal_malloc (sizeof(TEST_NODE_DATA_X), NULL);
      if (NULL != px_node_data)
      {
         (void) pal_memmove (px_node_data, &x_node_data,
            sizeof(*px_node_data));
         x_list_node_data.p_data = px_node_data;
         x_list_node_data.ui_data_size = sizeof(*px_node_data);
      }

      e_list_ret = list_node_append (hl_list2_hdl, &x_list_node_data);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         if (eLIST_RET_LIST_FULL == e_list_ret)
         {
            LIST_TEST_LOG("List Full");
            i_error = 0;
         }
         else
         {
            LIST_TEST_LOG("list_node_append failed: %d", e_list_ret);
         }

      }
      ui_i++;
   }

   printf ("\n\nSorted List2 contents: \n***************************************\n");
   list_print_all_nodes (hl_list2_hdl);
   e_list_ret = list_for_all_nodes (hl_list2_hdl, fn_for_all_cbk, NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
      }
   }
   printf ("\n***************************************\n");

   e_list_ret = list_sorted_merge (hl_list1_hdl, hl_list2_hdl,
      fn_list_compare_fn_cbk,
      NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      LIST_TEST_LOG("list_sorted_merge failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }

   printf ("\n\nSorted Merged List contents: \n***************************************\n");
   list_print_all_nodes (hl_list1_hdl);
   e_list_ret = list_for_all_nodes (hl_list1_hdl, fn_for_all_cbk, NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
      }
   }
   printf ("\n***************************************\n");

CLEAN_RETURN:
   return i_error;
}

int test_case_list_sort ()
{
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   LIST_INIT_PARAMS_X x_init_params = {0};
   LIST_HDL hl_list_hdl = NULL;
   // uint32_t uia_array1 [] = { 9,8,7,6,5,4,3,2,1 };

   uint32_t uia_array1 [] = { };
   uint32_t ui_n1 = sizeof(uia_array1) / sizeof(uint32_t);
   uint32_t ui_i = 0;
   TEST_NODE_DATA_X x_node_data = {0};
   TEST_NODE_DATA_X *px_node_data = NULL;
   LIST_NODE_DATA_X x_list_node_data = {0};
   int i_error = -1;
   LIST_SORT_PARAMS_X x_sort_params = {eLIST_SORT_ALGORITHM_INVALID};

   x_init_params.ui_list_max_elements = 20;
   e_list_ret = list_create (&hl_list_hdl, &x_init_params);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (NULL == hl_list_hdl))
   {
      LIST_TEST_LOG("list_create failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   LIST_TEST_LOG("list_create success");

   while (ui_i < ui_n1)
   {
      (void) pal_memset (&x_node_data, 0x00, sizeof(x_node_data));
      x_node_data.ui_timestamp = pal_get_system_time ();
      x_node_data.ui_value = (uint32_t) uia_array1 [ui_i];
      px_node_data = pal_malloc (sizeof(TEST_NODE_DATA_X), NULL);
      if (NULL != px_node_data)
      {
         (void) pal_memmove (px_node_data, &x_node_data,
            sizeof(*px_node_data));
         x_list_node_data.p_data = px_node_data;
         x_list_node_data.ui_data_size = sizeof(*px_node_data);
      }

      e_list_ret = list_node_append (hl_list_hdl, &x_list_node_data);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         if (eLIST_RET_LIST_FULL == e_list_ret)
         {
            LIST_TEST_LOG("List Full");
            i_error = 0;
         }
         else
         {
            LIST_TEST_LOG("list_node_append failed: %d", e_list_ret);
         }

      }

      ui_i++;
   }

   printf (
      "\n\nUnSorted List contents: \n***************************************\n");
   list_print_all_nodes (hl_list_hdl);
   e_list_ret = list_for_all_nodes (hl_list_hdl, fn_for_all_cbk,
      NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
      }
   }
   printf ("\n***************************************\n");

   x_sort_params.e_sort_algorithm = eLIST_SORT_ALGORITHM_MERGE_SORT;
   x_sort_params.fn_compare_fn_cbk = fn_list_compare_fn_cbk;
   e_list_ret = list_sort (hl_list_hdl, &x_sort_params);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      LIST_TEST_LOG("list_sorted_merge failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }

   printf (
      "\n\nSorted List contents: \n***************************************\n");
   list_print_all_nodes (hl_list_hdl);
   e_list_ret = list_for_all_nodes (hl_list_hdl, fn_for_all_cbk,
      NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
      }
   }
   printf ("\n***************************************\n");

CLEAN_RETURN:
   return i_error;
}

int main ()
{
   LIST_HDL hl_list_hdl = NULL;
   int i_error = -1;
   int i_ret = -1;
   LIST_RET_E e_list_ret = eLIST_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   LIST_INIT_PARAMS_X x_init_params = {0};
   bool b_pal_init = false;
   int i_command = -1;
   int i_value = -1;
   TEST_NODE_DATA_X x_node_data = {0};
   TEST_NODE_DATA_X *px_node_data = NULL;
   LIST_NODE_DATA_X x_list_node_data = {0};
   LIST_NODE_DATA_X x_list_search_node_data = {0};
   TEST_NODE_DATA_X x_test_search_node = {0};
   PAL_LOGGER_INIT_PARAMS_X x_logger_param = {false};

   x_logger_param.b_enable_console_logging = true;
   x_logger_param.e_level = eLOG_LEVEL_MEDIUM;
   pal_logger_env_init (&x_logger_param);

   e_pal_ret = pal_env_init();
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      LIST_TEST_LOG("pal_env_init failed: %d", e_pal_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   b_pal_init = true;
   LIST_TEST_LOG("pal_env_init success");

   x_init_params.ui_list_max_elements = 10;
   e_list_ret = list_create(&hl_list_hdl, &x_init_params);
   if ((eLIST_RET_SUCCESS != e_list_ret) || (NULL == hl_list_hdl))
   {
      LIST_TEST_LOG("list_create failed: %d", e_list_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   LIST_TEST_LOG("list_create success");

   e_list_ret = list_for_all_nodes (hl_list_hdl, fn_for_all_cbk, NULL);
   if (eLIST_RET_SUCCESS != e_list_ret)
   {
      if (eLIST_RET_LIST_EMPTY == e_list_ret)
      {
         LIST_TEST_LOG("List Empty");
         i_error = 0;
      }
      else
      {
         LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
         i_error = -1;
         goto CLEAN_RETURN;
      }
   }

   while (true)
   {
      (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
      printf ("Commands:\n\t1. Append\n\t2. Prepend\n\t3. Delete @ Head\n\t"
         "4. Delete @ Tail\n\t5. Search Node\n\t6. Delete Node\n\t"
         "7. Insert Sorted\n\t8. Split List\n"
         "Enter Command: ");
      i_ret = scanf ("%d", &i_command); (void) i_ret;

      if ((1 == i_command) || (2 == i_command) || (6 == i_command) || (7 == i_command))
      {
         printf ("\nEnter Value: ");
         i_ret = scanf ("%d", &i_value); (void) i_ret;

         if ((1 == i_command) || (2 == i_command) || (7 == i_command))
         {
            x_node_data.ui_timestamp = pal_get_system_time ();
            x_node_data.ui_value = (uint32_t) i_value;
            px_node_data = pal_malloc (sizeof(TEST_NODE_DATA_X), NULL );
            if (NULL != px_node_data)
            {
               (void) pal_memmove (px_node_data, &x_node_data,
                  sizeof(*px_node_data));
               x_list_node_data.p_data = px_node_data;
               x_list_node_data.ui_data_size = sizeof(*px_node_data);
            }
            else
            {
               continue;
            }
         }
      }
      switch (i_command)
      {
         case 1:
         {
            e_list_ret = list_node_append (hl_list_hdl, &x_list_node_data);
            if (eLIST_RET_SUCCESS != e_list_ret)
            {
               if (eLIST_RET_LIST_FULL == e_list_ret)
               {
                  LIST_TEST_LOG("List Full");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_append failed: %d", e_list_ret);
               }

            }
            break;
         }
         case 2:
         {
            e_list_ret = list_node_prepend (hl_list_hdl, &x_list_node_data);
            if (eLIST_RET_SUCCESS != e_list_ret)
            {
               if (eLIST_RET_LIST_FULL == e_list_ret)
               {
                  LIST_TEST_LOG("List Full");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_append failed: %d", e_list_ret);
               }

            }
            break;
         }
         case 3:
         {
            e_list_ret = list_node_delete_at_head (hl_list_hdl,
               &x_list_node_data);
            if ((eLIST_RET_SUCCESS != e_list_ret)
               || (NULL == x_list_node_data.p_data)
               || (0 == x_list_node_data.ui_data_size))
            {
               if (eLIST_RET_LIST_EMPTY == e_list_ret)
               {
                  LIST_TEST_LOG("List Empty");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_delete_at_head failed: %d",
                     e_list_ret);
               }
            }
            else
            {
               px_node_data = (TEST_NODE_DATA_X *) x_list_node_data.p_data;
               LIST_TEST_LOG("Deleted Node. Value: %d, Timestamp: %d",
                  px_node_data->ui_value, px_node_data->ui_timestamp);
               pal_free (px_node_data);
               px_node_data = NULL;
            }
            break;
         }
         case 4:
         {
            e_list_ret = list_node_delete_at_tail (hl_list_hdl,
               &x_list_node_data);
            if ((eLIST_RET_SUCCESS != e_list_ret)
               || (NULL == x_list_node_data.p_data)
               || (0 == x_list_node_data.ui_data_size))
            {
               if (eLIST_RET_LIST_EMPTY == e_list_ret)
               {
                  LIST_TEST_LOG("List Empty");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_delete_at_tail failed: %d",
                     e_list_ret);
               }
            }
            else
            {
               px_node_data = (TEST_NODE_DATA_X *) x_list_node_data.p_data;
               LIST_TEST_LOG("Deleted Node. Value: %d, Timestamp: %d",
                  px_node_data->ui_value, px_node_data->ui_timestamp);
               pal_free (px_node_data);
               px_node_data = NULL;
            }
            break;
         }
         case 6:
         {
            x_test_search_node.ui_value = (uint32_t) i_value;
            x_list_search_node_data.p_data = &x_test_search_node;
            x_list_search_node_data.ui_data_size = sizeof(x_test_search_node);
            e_list_ret = list_node_delete_after_search (hl_list_hdl,
               &x_list_search_node_data, fn_search_node_cbk, NULL,
               &x_list_node_data);
            if ((eLIST_RET_SUCCESS != e_list_ret)
               || (NULL == x_list_node_data.p_data)
               || (0 == x_list_node_data.ui_data_size))
            {
               if (eLIST_RET_LIST_EMPTY == e_list_ret)
               {
                  LIST_TEST_LOG("List Empty");
                  i_error = 0;
               }
               else if (eLIST_RET_LIST_NODE_NOT_FOUND == e_list_ret)
               {
                  LIST_TEST_LOG("List Node Not Found");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_delete_after_search failed: %d",
                     e_list_ret);
               }
            }
            else
            {
               px_node_data = (TEST_NODE_DATA_X *) x_list_node_data.p_data;
               LIST_TEST_LOG("Deleted Node. Value: %d, Timestamp: %d",
                  px_node_data->ui_value, px_node_data->ui_timestamp);
               pal_free (px_node_data);
               px_node_data = NULL;
            }
            break;
         }
         case 7:
         {
            e_list_ret = list_node_insert_sorted (hl_list_hdl, &x_list_node_data,
               fn_list_compare_fn_cbk, NULL);
            if (eLIST_RET_SUCCESS != e_list_ret)
            {
               if (eLIST_RET_LIST_FULL == e_list_ret)
               {
                  LIST_TEST_LOG("List Full");
                  i_error = 0;
               }
               else
               {
                  LIST_TEST_LOG("list_node_insert_sorted failed: %d", e_list_ret);
               }

            }
            break;
         }
         case 8:
         {
            LIST_HDL hl_front_list_hdl = NULL;
            LIST_HDL hl_back_list_hdl = NULL;
            e_list_ret = list_front_back_split (hl_list_hdl, &hl_front_list_hdl,
               &hl_back_list_hdl);
            if ((eLIST_RET_SUCCESS != e_list_ret) || (NULL == hl_front_list_hdl)
               || (NULL == hl_back_list_hdl))
            {
               LIST_TEST_LOG("list_front_back_split failed: %d", e_list_ret);
            }
            else
            {
               printf ("\n\nFront List contents: \n***************************************\n");
               list_print_all_nodes (hl_front_list_hdl);
               e_list_ret = list_for_all_nodes (hl_front_list_hdl, fn_for_all_cbk, NULL);
               if (eLIST_RET_SUCCESS != e_list_ret)
               {
                  if (eLIST_RET_LIST_EMPTY == e_list_ret)
                  {
                     LIST_TEST_LOG("List Empty");
                     i_error = 0;
                  }
                  else
                  {
                     LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
                     i_error = -1;
                  }
               }
               printf ("\n***************************************\n");

               printf ("\n\nBack List contents: \n***************************************\n");
               list_print_all_nodes (hl_back_list_hdl);
               e_list_ret = list_for_all_nodes (hl_back_list_hdl, fn_for_all_cbk, NULL);
               if (eLIST_RET_SUCCESS != e_list_ret)
               {
                  if (eLIST_RET_LIST_EMPTY == e_list_ret)
                  {
                     LIST_TEST_LOG("List Empty");
                     i_error = 0;
                  }
                  else
                  {
                     LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
                     i_error = -1;
                  }
               }
               printf ("\n***************************************\n");
            }
            break;
         }
         case 9:
         {
            test_case_list_sorted_merge ();
            break;
         }

         case 10:
         {
            test_case_list_sort ();
            break;
         }
         default:
         {
            LIST_TEST_LOG("Wrong Command: %d", i_command);
            break;
         }
      }

      printf ("\n\nList contents: \n***************************************\n");
      list_print_all_nodes (hl_list_hdl);
      e_list_ret = list_for_all_nodes (hl_list_hdl, fn_for_all_cbk, NULL);
      if (eLIST_RET_SUCCESS != e_list_ret)
      {
         if (eLIST_RET_LIST_EMPTY == e_list_ret)
         {
            LIST_TEST_LOG("List Empty");
            i_error = 0;
         }
         else
         {
            LIST_TEST_LOG("list_for_all_nodes failed: %d", e_list_ret);
            i_error = -1;
         }
      }
      printf ("\n***************************************\n");
   }


CLEAN_RETURN:
   if (0 != i_error)
   {
      if (NULL != hl_list_hdl)
      {
         e_list_ret = list_delete (hl_list_hdl);
         if (eLIST_RET_SUCCESS != e_list_ret)
         {
            LIST_TEST_LOG("list_delete failed: %d", e_list_ret);
         }
         else
         {
            LIST_TEST_LOG("list_delete success");
         }
      }
   }
   if (true == b_pal_init)
   {
      e_pal_ret = pal_env_deinit();
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         LIST_TEST_LOG("pal_env_deinit failed: %d", e_pal_ret);
      }
      else
      {
         LIST_TEST_LOG("pal_env_deinit success");
      }
   }
   return i_error;
}
