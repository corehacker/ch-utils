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
 * \file   gentree_private.h
 *
 * \author sandeepprakash
 *
 * \date   25-Oct-2012
 *
 * \brief
 *
 ******************************************************************************/

#ifndef __GENTREE_PRIVATE_H__
#define __GENTREE_PRIVATE_H__

#ifdef  __cplusplus
extern  "C"
{
#endif

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/*********************** CLASS/STRUCTURE/UNION DATA TYPES *********************/
typedef struct _GENTREE_NODE_X GENTREE_NODE_X;

struct _GENTREE_NODE_X
{
   GENTREE_NODE_DATA_X x_node_data;

   GENTREE_NODE_X **ppx_children;
};

typedef struct _GENTREE_CTXT_X
{
   GENTREE_INIT_PARAMS_X x_init_params;

   uint32_t ui_node_count;

   GENTREE_NODE_X *px_root;
} GENTREE_CTXT_X;

/***************************** FUNCTION PROTOTYPES ****************************/

#ifdef   __cplusplus
}
#endif

#endif /* __GENTREE_PRIVATE_H__ */
