/*******************************************************************************
 * Copyright (c) 2012, Sandeep Prakash <123sandy@gmail.com>
 * All rights reserved.
 *
 * \file   q_test.c
 * \author sandeepprakash
 *
 * \date   25-Sep-2012
 *
 * \brief  
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>

#include <ch-utils/exp_q.h>

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define Q_TEST_LOG(format,...)                                      \
   do                                                                      \
   {                                                                       \
      printf("%s: %d: "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);                                           \
   } while (0)
/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/
typedef struct _TEST_NODE_DATA_X
{
   uint32_t ui_value;

   uint32_t ui_timestamp;
} TEST_NODE_DATA_X;

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
int main ()
{
   Q_HDL hl_q_hdl = NULL;
   int i_error = -1;
   int i_ret = -1;
   Q_RET_E e_q_ret = eQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   Q_PARAMS_X x_q_params = {0};
   bool b_pal_init = false;
   int i_command = -1;
   TEST_NODE_DATA_X x_node_data = {0};
   TEST_NODE_DATA_X *px_node_data = NULL;
   Q_NODE_DATA_X x_q_node_data = {0};

   e_pal_ret = pal_env_init();
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      Q_TEST_LOG("pal_env_init failed: %d", e_pal_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   b_pal_init = true;
   Q_TEST_LOG("pal_env_init success");

   x_q_params.ui_max_elements = 5;
   e_q_ret = q_init(&hl_q_hdl, &x_q_params);
   if ((eQ_RET_SUCCESS != e_q_ret) || (NULL == hl_q_hdl))
   {
      Q_TEST_LOG("q_init failed: %d", e_q_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   Q_TEST_LOG("q_init success");

   while (true)
   {
      (void) pal_memset(&x_node_data, 0x00, sizeof(x_node_data));
      printf ("Commands:\n\t1. ENQ\n\t2. DEQ\nEnter Command: ");
      i_ret =scanf ("%d", &i_command); (void) i_ret;

      if (1 == i_command)
      {
         printf ("\nEnter Value: ");
         i_ret = scanf ("%d", &(x_node_data.ui_value)); (void) i_ret;
         x_node_data.ui_timestamp = pal_get_system_time ();

         px_node_data = pal_malloc (sizeof(TEST_NODE_DATA_X), NULL);
         if (NULL != px_node_data)
         {
            (void) pal_memmove (px_node_data, &x_node_data,
               sizeof(*px_node_data));
            x_q_node_data.p_data = px_node_data;
            x_q_node_data.ui_data_size = sizeof(*px_node_data);
         }
         else
         {
            continue;
         }
      }
      switch (i_command)
      {
         case 1:
         {
            e_q_ret = q_enq (hl_q_hdl, &x_q_node_data);
            if (eQ_RET_SUCCESS != e_q_ret)
            {
               if (eQ_RET_Q_FULL == e_q_ret)
               {
                  Q_TEST_LOG("Q Full");
                  i_error = 0;
               }
               else
               {
                  Q_TEST_LOG("q_enq failed: %d", e_q_ret);
               }
            }
            break;
         }
         case 2:
         {
            e_q_ret = q_deq (hl_q_hdl, &x_q_node_data);
            if ((eQ_RET_SUCCESS != e_q_ret)
               || (NULL == x_q_node_data.p_data)
               || (0 == x_q_node_data.ui_data_size))
            {
               if (eQ_RET_Q_EMPTY == e_q_ret)
               {
                  Q_TEST_LOG("Q Empty");
                  i_error = 0;
               }
               else
               {
                  Q_TEST_LOG("q_deq failed: %d", e_q_ret);
               }
            }
            else
            {
               px_node_data = (TEST_NODE_DATA_X *) x_q_node_data.p_data;
               Q_TEST_LOG("Deleted Node. Value: %d, Timestamp: %d",
                  px_node_data->ui_value, px_node_data->ui_timestamp);
            }
            break;
         }
         default:
         {
            Q_TEST_LOG("Wrong Command: %d", i_command);
            break;
         }
      }
   }
CLEAN_RETURN:
   if (0 != i_error)
   {
      if (NULL != hl_q_hdl)
      {
         e_q_ret = q_deinit (hl_q_hdl);
         if (eQ_RET_SUCCESS != e_q_ret)
         {
            Q_TEST_LOG("q_deinit failed: %d", e_q_ret);
         }
         else
         {
            Q_TEST_LOG("q_deinit success");
         }
      }
   }
   if (true == b_pal_init)
   {
      e_pal_ret = pal_env_deinit ();
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         Q_TEST_LOG("pal_env_deinit failed: %d", e_pal_ret);
      }
      else
      {
         Q_TEST_LOG("pal_env_deinit success");
      }
   }
   return i_error;
}
