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
 * \file   sock_utils_private.h
 *
 * \author sandeepprakash
 *
 * \date   24-Sep-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __SOCK_UTILS_PRIVATE_H__
#define __SOCK_UTILS_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define SOCK_UTIL_LOG_STR                       "SUTL"

#define SOCK_UTIL_LOG_LOW(format,...)                                         \
do                                                                            \
{                                                                             \
   LOG_LOW (SOCK_UTIL_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,          \
      ##__VA_ARGS__);                                                         \
} while (0)

#define SOCK_UTIL_LOG_MED(format,...)                                         \
do                                                                            \
{                                                                             \
   LOG_MED (SOCK_UTIL_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,          \
      ##__VA_ARGS__);                                                         \
} while (0)

#define SOCK_UTIL_LOG_HIGH(format,...)                                        \
do                                                                            \
{                                                                             \
   LOG_HIGH (SOCK_UTIL_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,         \
      ##__VA_ARGS__);                                                         \
} while (0)

#define SOCK_UTIL_LOG_FULL(format,...)                                        \
do                                                                            \
{                                                                             \
   LOG_FULL (SOCK_UTIL_LOG_STR,__FILE__,__FUNCTION__,__LINE__,format,         \
      ##__VA_ARGS__);                                                         \
} while (0)

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/

/***************************** FUNCTION PROTOTYPES ****************************/

#ifdef   __cplusplus
}
#endif

#endif /* __SOCK_UTILS_PRIVATE_H__ */
