/************************************************************************
 *   Copyright (C) 2000,2001 Edward Brocklesby
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: tools.h,v 1.2 2001/02/11 08:00:18 ejb Exp $
 */

/*
 * tools.h
 *
 * Definitions/prototypes for src/tools.c
 *
 * Adrian Chadd <adrian@creative.net.au>
 */
#ifndef __TOOLS_H__
#define __TOOLS_H__


/*
 * double-linked-list stuff
 */
typedef struct _dlink_node dlink_node;
typedef struct _dlink_list dlink_list;

struct _dlink_node {
    void *data;
    dlink_node *prev;
    dlink_node *next;

};
  
struct _dlink_list {
    dlink_node *head;
    dlink_node *tail;
};

dlink_node *make_dlink_node();
void dlinkAdd(void *data, dlink_node * m, dlink_list * list);
void dlinkAddBefore(dlink_node *b, void *data, dlink_node *m, dlink_list *list);
void dlinkAddTail(void *data, dlink_node *m, dlink_list *list);
void dlinkDelete(dlink_node *m, dlink_list *list);
void dlinkMoveList(dlink_list *from, dlink_list *to);
int dlink_list_length(dlink_list *m);
dlink_node *dlinkFind(dlink_list *m, void *data);


#endif
