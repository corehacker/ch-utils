/*******************************************************************************
 * Copyright (c) 2012, Sandeep Prakash <123sandy@gmail.com>
 * All rights reserved.
 *
 * \file   msgq_test.c
 * \author sandeepprakash
 *
 * \date   25-Sep-2012
 *
 * \brief  
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>

#include <ch-utils/exp_msgq.h>
#include <ch-utils/exp_task.h>

/********************************* CONSTANTS **********************************/

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define MSGQ_TEST_LOG(format,...)                                      \
   do                                                                      \
   {                                                                       \
      printf("%s: %d: "format"\n",__FUNCTION__,__LINE__,##__VA_ARGS__);                                           \
   } while (0)
/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/
typedef struct _MSGQ_TEST_CTXT_X
{
   MSGQ_HDL hl_msgq_hdl;

   TASK_HDL hl_task_hdl;
} MSGQ_TEST_CTXT_X;

typedef struct _MSGQ_POST_DATA_X
{
   uint32_t ui_user_int;

   uint32_t ui_timestamp;
} MSGQ_POST_DATA_X;

/************************ STATIC FUNCTION PROTOTYPES **************************/
static void *msgq_test_task (
      void *p_thread_args);

/****************************** LOCAL FUNCTIONS *******************************/
static void *msgq_test_task (
      void *p_thread_args)
{
   MSGQ_TEST_CTXT_X *px_test_ctxt = NULL;
   MSGQ_RET_E e_msgq_ret = eMSGQ_RET_FAILURE;
   MSGQ_DATA_X x_msgq_data = {NULL};
   MSGQ_POST_DATA_X *px_post_data = NULL;

   if (NULL == p_thread_args)
   {
      goto CLEAN_RETURN;
   }

   px_test_ctxt = (MSGQ_TEST_CTXT_X *) p_thread_args;

   // pal_sleep(2000);
   while (1)
   {
      MSGQ_TEST_LOG("%d :: MSGQ Task Loop", pal_get_system_time());

      e_msgq_ret = msgq_get_msg (px_test_ctxt->hl_msgq_hdl, &x_msgq_data, 5000);
      if (eMSGQ_RET_SUCCESS != e_msgq_ret)
      {
         if (eMSGQ_RET_OP_TIMEDOUT == e_msgq_ret)
         {
            MSGQ_TEST_LOG("msgq_get_msg timedout: %d", e_msgq_ret);
         }
         else
         {
            MSGQ_TEST_LOG("msgq_get_msg failed: %d", e_msgq_ret);
         }
      }
      else
      {
         MSGQ_TEST_LOG("msgq_get_msg success");

         if ((NULL != x_msgq_data.p_data) && (0 != x_msgq_data.ui_data_size))
         {
            px_post_data = x_msgq_data.p_data;

            MSGQ_TEST_LOG("Got %p, %d, %d", px_post_data,
               px_post_data->ui_user_int, px_post_data->ui_timestamp);
         }
         else
         {
            MSGQ_TEST_LOG("msgq_get_msg success. But got wrong pointers.");
         }
      }
   }

CLEAN_RETURN:
   return p_thread_args;
}

int main ()
{
   int i_error = -1;
   int i_ret = -1;
   MSGQ_RET_E e_msgq_ret = eMSGQ_RET_FAILURE;
   PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
   MSGQ_INIT_PARAMS_X x_msgq_params = {0};
   bool b_pal_init = false;
   int i_command = -1;
   MSGQ_TEST_CTXT_X *px_test_ctxt = NULL;
   TASK_RET_E e_task_ret = eTASK_RET_FAILURE;
   TASK_CREATE_PARAM_X x_task_param = {{0}};
   MSGQ_POST_DATA_X *px_post_data = NULL;
   MSGQ_DATA_X x_msgq_data = {NULL};

   e_pal_ret = pal_env_init();
   if (ePAL_RET_SUCCESS != e_pal_ret)
   {
      MSGQ_TEST_LOG("pal_env_init failed: %d", e_pal_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   b_pal_init = true;
   MSGQ_TEST_LOG("pal_env_init success");

   px_test_ctxt = pal_malloc(sizeof(MSGQ_TEST_CTXT_X), NULL);

   x_msgq_params.ui_msgq_size = 3;
   e_msgq_ret = msgq_init(&(px_test_ctxt->hl_msgq_hdl), &x_msgq_params);
   if ((eMSGQ_RET_SUCCESS != e_msgq_ret) || (NULL == px_test_ctxt->hl_msgq_hdl))
   {
      MSGQ_TEST_LOG("msgq_init failed: %d", e_msgq_ret);
      i_error = -1;
      goto CLEAN_RETURN;
   }
   MSGQ_TEST_LOG("msgq_init success");

   x_task_param.p_app_data = px_test_ctxt;
   x_task_param.fn_task_routine = msgq_test_task;
   e_task_ret = task_create(&(px_test_ctxt->hl_task_hdl), &x_task_param);
   e_task_ret = task_start(px_test_ctxt->hl_task_hdl);
   while (true)
   {
      printf ("Post Number To Task: ");
      i_ret =scanf ("%d", &i_command); (void) i_ret;

      px_post_data = pal_malloc(sizeof(MSGQ_POST_DATA_X), NULL);

      px_post_data->ui_user_int = i_command;
      px_post_data->ui_timestamp = pal_get_system_time();


      x_msgq_data.p_data = px_post_data;
      x_msgq_data.ui_data_size = sizeof(px_post_data);

      MSGQ_TEST_LOG("Posting %p, %d, %d to Q", px_post_data,
         px_post_data->ui_user_int, px_post_data->ui_timestamp);

      e_msgq_ret = msgq_add_msg (px_test_ctxt->hl_msgq_hdl, &x_msgq_data, 5000);
      if (eMSGQ_RET_SUCCESS != e_msgq_ret)
      {
         if (eMSGQ_RET_OP_TIMEDOUT == e_msgq_ret)
         {
            MSGQ_TEST_LOG("msgq_add_msg timedout: %d", e_msgq_ret);
         }
         else
         {
            MSGQ_TEST_LOG("msgq_add_msg failed: %d", e_msgq_ret);
         }
      }
      else
      {
         MSGQ_TEST_LOG("msgq_add_msg success");
      }
   }
CLEAN_RETURN:
   if (NULL != px_test_ctxt)
   {
      if (NULL != px_test_ctxt->hl_msgq_hdl)
      {
         e_msgq_ret = msgq_deinit (px_test_ctxt->hl_msgq_hdl);
         if (eMSGQ_RET_SUCCESS != e_msgq_ret)
         {
            MSGQ_TEST_LOG("msgq_deinit failed: %d", e_msgq_ret);
         }
         else
         {
            MSGQ_TEST_LOG("msgq_deinit success");
         }
      }

      pal_free (px_test_ctxt);
      px_test_ctxt = NULL;
   }
   if (true == b_pal_init)
   {
      e_pal_ret = pal_env_deinit ();
      if (ePAL_RET_SUCCESS != e_pal_ret)
      {
         MSGQ_TEST_LOG("pal_env_deinit failed: %d", e_pal_ret);
      }
      else
      {
         MSGQ_TEST_LOG("pal_env_deinit success");
      }
   }
   return i_error;
}
