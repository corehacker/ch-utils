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
 * Copyright (c) 2012, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   sock_utils.c
 *
 * \author sandeepprakash
 *
 * \date   23-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_sock_utils.h>
#include "sock_utils_private.h"

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define PAL_SOCK_LISTEN_BACKLOG           (5)

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/

/****************************** LOCAL FUNCTIONS *******************************/
PAL_RET_E tcp_listen_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   uint16_t us_listen_port_ho)
{
   PAL_RET_E e_error = ePAL_RET_FAILURE;
   PAL_RET_E e_error_pvt = ePAL_RET_FAILURE;
   PAL_SOCK_ADDR_IN_X x_sock_addr = {0};
   PAL_SOCK_REUSEADDR_DATA_X x_reuseaddr_data = {0};

   if ((NULL == phl_sock_hdl) || (0 == us_listen_port_ho))
   {
      SOCK_UTIL_LOG_MED("Invalid Args");
      e_error = ePAL_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *phl_sock_hdl = NULL;

   e_error = pal_sock_create(phl_sock_hdl, ePAL_SOCK_DOMAIN_AF_INET,
      ePAL_SOCK_TYPE_SOCK_STREAM, ePAL_SOCK_PROTOCOL_DEFAULT);
   if ((ePAL_RET_SUCCESS != e_error) || (NULL == *phl_sock_hdl))
   {
      SOCK_UTIL_LOG_MED("pal_sock_create failed: %d", e_error);
      if (ePAL_RET_SUCCESS == e_error)
      {
         e_error = ePAL_RET_SOCK_CREATE_FAILED;
      }
      goto CLEAN_RETURN;
   }

   x_reuseaddr_data.ui_opt_val = 1;
   e_error = pal_setsockopt (*phl_sock_hdl, ePAL_SOCK_OPT_SO_REUSEADDR,
      &x_reuseaddr_data, sizeof(x_reuseaddr_data));
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_setsockopt failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   x_sock_addr.us_sin_port_no = pal_htons(us_listen_port_ho);
   x_sock_addr.x_sin_addr.ui_ip_addr_no = pal_htonl(PAL_SOCK_IN_ADDR_ANY);
   e_error = pal_sock_bind (*phl_sock_hdl, &x_sock_addr);
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_sock_bind failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   e_error = pal_sock_listen (*phl_sock_hdl, PAL_SOCK_LISTEN_BACKLOG);
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_sock_listen failed: %d", e_error);
   }
   else
   {
      SOCK_UTIL_LOG_HIGH("TCP socket listening on port: %d", us_listen_port_ho);
   }
CLEAN_RETURN:
   if (ePAL_RET_SUCCESS != e_error)
   {
      if ((NULL != phl_sock_hdl) && (NULL != *phl_sock_hdl))
      {
         e_error_pvt = pal_sock_close (*phl_sock_hdl);
         if (ePAL_RET_SUCCESS != e_error_pvt)
         {
            SOCK_UTIL_LOG_MED("pal_sock_close failed: %d", e_error_pvt);
         }
         *phl_sock_hdl = NULL;
      }
   }
   return e_error;
}

PAL_RET_E tcp_listen_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl)
{
   return pal_sock_close(hl_sock_hdl);
}

PAL_RET_E udp_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   uint16_t us_port_ho)
{
   PAL_RET_E e_error = ePAL_RET_FAILURE;
   PAL_RET_E e_error_pvt = ePAL_RET_FAILURE;
   PAL_SOCK_ADDR_IN_X x_sock_addr = {0};
   PAL_SOCK_REUSEADDR_DATA_X x_reuseaddr_data = {0};

   if ((NULL == phl_sock_hdl) || (0 == us_port_ho))
   {
      SOCK_UTIL_LOG_MED("Invalid Args");
      e_error = ePAL_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *phl_sock_hdl = NULL;

   e_error = pal_sock_create(phl_sock_hdl, ePAL_SOCK_DOMAIN_AF_INET,
      ePAL_SOCK_TYPE_SOCK_DGRAM, ePAL_SOCK_PROTOCOL_DEFAULT);
   if ((ePAL_RET_SUCCESS != e_error) || (NULL == *phl_sock_hdl))
   {
      SOCK_UTIL_LOG_MED("pal_sock_create failed: %d", e_error);
      if (ePAL_RET_SUCCESS == e_error)
      {
         e_error = ePAL_RET_SOCK_CREATE_FAILED;
      }
      goto CLEAN_RETURN;
   }

   x_reuseaddr_data.ui_opt_val = 1;
   e_error = pal_setsockopt (*phl_sock_hdl, ePAL_SOCK_OPT_SO_REUSEADDR,
      &x_reuseaddr_data, sizeof(x_reuseaddr_data));
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_setsockopt failed: %d", e_error);
      goto CLEAN_RETURN;
   }

   x_sock_addr.us_sin_port_no = pal_htons(us_port_ho);
   x_sock_addr.x_sin_addr.ui_ip_addr_no = pal_htonl(PAL_SOCK_IN_ADDR_ANY);
   e_error = pal_sock_bind (*phl_sock_hdl, &x_sock_addr);
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_sock_bind failed: %d", e_error);
   }
   else
   {
      SOCK_UTIL_LOG_HIGH("UDP socket waiting on port: %d", us_port_ho);
   }
CLEAN_RETURN:
   if (ePAL_RET_SUCCESS != e_error)
   {
      if ((NULL != phl_sock_hdl) && (NULL != *phl_sock_hdl))
      {
         e_error_pvt = pal_sock_close (*phl_sock_hdl);
         if (ePAL_RET_SUCCESS != e_error_pvt)
         {
            SOCK_UTIL_LOG_MED("pal_sock_close failed: %d", e_error_pvt);
         }
         *phl_sock_hdl = NULL;
      }
   }
   return e_error;
}

PAL_RET_E udp_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl)
{
   return pal_sock_close(hl_sock_hdl);
}

PAL_RET_E tcp_connect_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   SOCK_UTIL_HOST_INFO_X *px_host_info,
   uint32_t ui_timeout_ms)
{
   PAL_RET_E e_error = ePAL_RET_FAILURE;
   PAL_RET_E e_error_pvt = ePAL_RET_FAILURE;
   PAL_SOCK_ADDR_IN_X x_connect_addr = {0};
   PAL_SOCK_IN_ADDR_X *px_in_addr = NULL;
   PAL_SOCK_ADDR_LIST_X x_sock_addr_list = {0};
   uint32_t ui_i = 0;
   uint8_t uca_ip_addr_str[16] = {0};

   if ((NULL == phl_sock_hdl) || (NULL == px_host_info))
   {
      SOCK_UTIL_LOG_MED("Invalid Args");
      e_error = ePAL_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   if (!(eSOCK_UTIL_HOST_INFO_DNS_NAME_BM & px_host_info->ui_bitmask)
      && !(eSOCK_UTIL_HOST_INFO_HOST_IP_BM & px_host_info->ui_bitmask))
   {
      SOCK_UTIL_LOG_MED("Invalid Args");
      e_error = ePAL_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   *phl_sock_hdl = NULL;

   e_error = pal_sock_create(phl_sock_hdl, ePAL_SOCK_DOMAIN_AF_INET,
      ePAL_SOCK_TYPE_SOCK_STREAM, ePAL_SOCK_PROTOCOL_DEFAULT);
   if ((ePAL_RET_SUCCESS != e_error) || (NULL == *phl_sock_hdl))
   {
      SOCK_UTIL_LOG_MED("pal_sock_create failed: %d", e_error);
      if (ePAL_RET_SUCCESS == e_error)
      {
         e_error = ePAL_RET_SOCK_CREATE_FAILED;
      }
      goto CLEAN_RETURN;
   }

   if ((eSOCK_UTIL_HOST_INFO_DNS_NAME_BM & px_host_info->ui_bitmask)
         && (NULL != px_host_info->puc_dns_name_str))
   {
      SOCK_UTIL_LOG_MED("Trying with DNS name: %s",
         px_host_info->puc_dns_name_str);
      e_error = pal_getaddrinfo (px_host_info->puc_dns_name_str,
         &x_sock_addr_list);
      if ((ePAL_RET_SUCCESS != e_error)
         || (NULL == x_sock_addr_list.ppx_addr_list)
         || (0 == x_sock_addr_list.ui_count))
      {
         SOCK_UTIL_LOG_MED("DNS lookup failed: %d.", e_error);
         if ((eSOCK_UTIL_HOST_INFO_HOST_IP_BM & px_host_info->ui_bitmask)
               && (0 != px_host_info->ui_host_ip_ho))
         {
            SOCK_UTIL_LOG_MED("Trying with provided IP address: 0x%X",
               px_host_info->ui_host_ip_ho);
            x_sock_addr_list.ppx_addr_list = pal_malloc (
               (sizeof(PAL_SOCK_IN_ADDR_X *)), NULL);
            if (NULL == x_sock_addr_list.ppx_addr_list)
            {
               SOCK_UTIL_LOG_MED("pal_malloc failed");
               e_error = ePAL_RET_OUT_OF_MEMORY;
               goto CLEAN_RETURN;
            }

            x_sock_addr_list.ui_count = 1;
            x_sock_addr_list.ppx_addr_list [0] = pal_malloc (
               sizeof(PAL_SOCK_IN_ADDR_X), NULL );
            if (NULL == x_sock_addr_list.ppx_addr_list [0])
            {
               e_error = ePAL_RET_OUT_OF_MEMORY;
               goto CLEAN_RETURN;
            }
            px_in_addr = x_sock_addr_list.ppx_addr_list [0];
            px_in_addr->ui_ip_addr_no = pal_htonl (
               px_host_info->ui_host_ip_ho);
         }
         else
         {
            SOCK_UTIL_LOG_MED("NO IP address provided. Bailing out.");
            goto CLEAN_RETURN;
         }
      }
      else
      {
         SOCK_UTIL_LOG_MED("Got %d IP addresses", x_sock_addr_list.ui_count);
      }
   }
   else if ((eSOCK_UTIL_HOST_INFO_HOST_IP_BM & px_host_info->ui_bitmask)
         && (0 != px_host_info->ui_host_ip_ho))
   {
      SOCK_UTIL_LOG_MED("Trying with provided IP address: 0x%X",
         px_host_info->ui_host_ip_ho);
      x_sock_addr_list.ppx_addr_list = pal_malloc (
         (sizeof(PAL_SOCK_IN_ADDR_X) * 1), NULL);
      if (NULL == x_sock_addr_list.ppx_addr_list)
      {
         SOCK_UTIL_LOG_MED("pal_malloc failed");
         e_error = ePAL_RET_OUT_OF_MEMORY;
         goto CLEAN_RETURN;
      }
      x_sock_addr_list.ui_count = 1;
      x_sock_addr_list.ppx_addr_list [0] = pal_malloc (
         sizeof(PAL_SOCK_IN_ADDR_X), NULL );
      if (NULL == x_sock_addr_list.ppx_addr_list [0])
      {
         e_error = ePAL_RET_OUT_OF_MEMORY;
         goto CLEAN_RETURN;
      }
      px_in_addr = x_sock_addr_list.ppx_addr_list [0];
      px_in_addr->ui_ip_addr_no = pal_htonl (
         px_host_info->ui_host_ip_ho);
   }
   else
   {
      SOCK_UTIL_LOG_MED("Invalid Args");
      e_error = ePAL_RET_INVALID_ARGS;
      goto CLEAN_RETURN;
   }

   for (ui_i = 0; ui_i < x_sock_addr_list.ui_count; ui_i++)
   {
      px_in_addr = x_sock_addr_list.ppx_addr_list [ui_i];
      (void) pal_get_ip_addr_str (px_in_addr, uca_ip_addr_str,
         sizeof(uca_ip_addr_str));
      SOCK_UTIL_LOG_MED("IP address %d: %s (0x%X)", ui_i + 1, uca_ip_addr_str,
         (px_in_addr->ui_ip_addr_no));
   }

   if ((eSOCK_UTIL_HOST_INFO_HOST_PORT_BM & px_host_info->ui_bitmask)
      && (0 != px_host_info->us_host_port_ho))
   {
      x_connect_addr.us_sin_port_no = pal_htons (px_host_info->us_host_port_ho);
   }
   else
   {
      x_connect_addr.us_sin_port_no = pal_htons (80);
   }
   x_connect_addr.x_sin_addr.ui_ip_addr_no =
      x_sock_addr_list.ppx_addr_list [0]->ui_ip_addr_no;
   e_error = pal_sock_connect (*phl_sock_hdl, &x_connect_addr,
      ePAL_SOCK_CONN_MODE_NON_BLOCKING, ui_timeout_ms);
   if (ePAL_RET_SUCCESS != e_error)
   {
      SOCK_UTIL_LOG_MED("pal_sock_connect failed: %d", e_error);
   }
CLEAN_RETURN:
   if (NULL != x_sock_addr_list.ppx_addr_list)
   {
      for (ui_i = 0; ui_i < x_sock_addr_list.ui_count; ui_i++)
      {
         pal_free (x_sock_addr_list.ppx_addr_list[ui_i]);
         x_sock_addr_list.ppx_addr_list[ui_i] = NULL;
      }
      pal_free (x_sock_addr_list.ppx_addr_list);
      x_sock_addr_list.ppx_addr_list = NULL;
   }
   if (ePAL_RET_SUCCESS != e_error)
   {
      if ((NULL != phl_sock_hdl) && (NULL != *phl_sock_hdl))
      {
         e_error_pvt = pal_sock_close (*phl_sock_hdl);
         if (ePAL_RET_SUCCESS != e_error_pvt)
         {
            SOCK_UTIL_LOG_MED("pal_sock_close failed: %d", e_error_pvt);
         }
         *phl_sock_hdl = NULL;
      }
   }
   return e_error;
}

PAL_RET_E tcp_connect_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl)
{
   return pal_sock_close(hl_sock_hdl);
}
