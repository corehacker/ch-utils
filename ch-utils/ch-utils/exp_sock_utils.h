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
 * \file   exp_sock_utils.h
 *
 * \author sandeepprakash
 *
 * \date   23-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __EXP_SOCK_UTILS_H__
#define __EXP_SOCK_UTILS_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/
typedef enum _SOCK_UTIL_HOST_INFO_BM_E
{
   eSOCK_UTIL_HOST_INFO_INVALID_BM        = 0x00000000,

   eSOCK_UTIL_HOST_INFO_DNS_NAME_BM       = 0x00000001,

   eSOCK_UTIL_HOST_INFO_HOST_IP_BM        = 0x00000002,

   eSOCK_UTIL_HOST_INFO_HOST_PORT_BM      = 0x00000004,

   eSOCK_UTIL_HOST_INFO_MAX_BM
} SOCK_UTIL_HOST_INFO_BM_E;

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _SOCK_UTIL_HOST_INFO_X
{
   uint32_t ui_bitmask;
   /*!< #_SOCK_UTIL_HOST_INFO_BM_E. */

   uint8_t *puc_dns_name_str;

   uint32_t ui_host_ip_ho;

   uint16_t us_host_port_ho;
} SOCK_UTIL_HOST_INFO_X;

/***************************** FUNCTION PROTOTYPES ****************************/
PAL_RET_E tcp_listen_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   uint16_t us_listen_port_ho);

PAL_RET_E tcp_listen_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl);

PAL_RET_E udp_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   uint16_t us_port_ho);

PAL_RET_E udp_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl);

PAL_RET_E tcp_connect_sock_create (
   PAL_SOCK_HDL *phl_sock_hdl,
   SOCK_UTIL_HOST_INFO_X *px_host_info,
   uint32_t ui_connect_timeout_ms);

PAL_RET_E tcp_connect_sock_delete (
   PAL_SOCK_HDL hl_sock_hdl);

#ifdef   __cplusplus
}
#endif

#endif /* __EXP_SOCK_UTILS_H__ */
