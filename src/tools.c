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
 * $Id: tools.c,v 1.2 2001/02/11 08:00:23 ejb Exp $
 */

/*
 * tools.c
 * 
 * Useful stuff, ripped from places ..
 *
 * adrian chadd <adrian@creative.net.au>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tools.h"


dlink_node *make_dlink_node()
{
	dlink_node *lp;

	lp = malloc(sizeof(dlink_node));
	
	lp->next = NULL;
	lp->prev = NULL;

	return lp;
}

/* 
 * dlink_ routines are stolen from squid, except for dlinkAddBefore,
 * which is mine.
 *   -- adrian
 */
void
dlinkAdd(void *data, dlink_node * m, dlink_list * list)
{
    m->data = data;
    m->prev = NULL;
    m->next = list->head;
    if (list->head)
        list->head->prev = m;
    list->head = m;
    if (list->tail == NULL)
        list->tail = m;
}

void
dlinkAddBefore(dlink_node *b, void *data, dlink_node *m, dlink_list *list)
{
    /* Shortcut - if its the first one, call dlinkAdd only */
    if (b == list->head)
        dlinkAdd(data, m, list);
    else {
        m->data = data;
        b->prev->next = m;
        m->prev = b->prev;
        b->prev = m; 
        m->next = b;
    }
}

void
dlinkAddTail(void *data, dlink_node *m, dlink_list *list)
{
    m->data = data;
    m->next = NULL;
    m->prev = list->tail;
    if (list->tail)
        list->tail->next = m;
    list->tail = m;
    if (list->head == NULL)
        list->head = m;
}

void
dlinkDelete(dlink_node *m, dlink_list *list)
{
    if (m->next)
        m->next->prev = m->prev;
    if (m->prev)
        m->prev->next = m->next;

    if (m == list->head)
        list->head = m->next;
    if (m == list->tail)
        list->tail = m->prev;
        
    m->next = m->prev = NULL;
}


/* 
 * dlink_list_length
 * inputs	- pointer to a dlink_list
 * output	- return the length (>=0) of a chain of links.
 * side effects	-
 */
extern int dlink_list_length(dlink_list *list)
{
  dlink_node *ptr;
  int   count = 0;

  for (ptr = list->head; ptr; ptr = ptr->next)
    count++;
  return count;
}

/*
 * dlinkFind
 * inputs	- list to search 
 *		- data
 * output	- pointer to link or NULL if not found
 * side effects	- Look for ptr in the linked listed pointed to by link.
 */
dlink_node *dlinkFind(dlink_list *list, void * data )
{
  dlink_node *ptr;

  for (ptr = list->head; ptr; ptr = ptr->next)
    {
      if (ptr->data == data)
	return (ptr);
    }
  return (NULL);
}

void
dlinkMoveList(dlink_list *from, dlink_list *to)
{
  /* There are three cases */
  /* case one, nothing in from list */

    if(from->head == NULL)
      return;

  /* case two, nothing in to list */
  /* actually if to->head is NULL and to->tail isn't, thats a bug */

    if(to->head == NULL) {
       to->head = from->head;
       to->tail = from->tail;
       from->head = from->tail = NULL;
       return;
    }

  /* third case play with the links */

    from->tail->next = to->head;
    from->head->prev = to->head->prev;
    to->head->prev = from->tail;
    to->head = from->head;
    from->head = from->tail = NULL;

  /* I think I got that right */
}


