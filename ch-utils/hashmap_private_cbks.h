/*******************************************************************************
 *  Repository for C modules.
 *  Copyright (C) 2014 Sandeep Prakash
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
 * Copyright (c) 2014, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   hashmap_private_cbks.h
 *
 * \author sandeepprakash
 *
 * \date   Mar 3, 2014
 *
 * \brief  
 *
 ******************************************************************************/

#ifndef __HASHMAP_PRIVATE_CBKS_H__
#define __HASHMAP_PRIVATE_CBKS_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/

/***************************** FUNCTION PROTOTYPES ****************************/
LIST_RET_E hm_list_for_all_cbk (
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

LIST_RET_E hm_print_nodes_for_all_cbk (
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

LIST_RET_E hm_list_search_cbk (
   LIST_NODE_DATA_X *px_search_node_data,
   LIST_NODE_DATA_X *px_curr_node_data,
   void *p_app_data);

LIST_RET_E hm_list_compare_fn_cbk (
   LIST_NODE_DATA_X *px_node_a_data,
   LIST_NODE_DATA_X *px_node_b_data,
   void *p_app_data);

#ifdef   __cplusplus
}
#endif

#endif /* __HASHMAP_PRIVATE_CBKS_H__ */
