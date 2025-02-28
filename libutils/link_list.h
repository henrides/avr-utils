#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

#include <avr/io.h>

typedef struct _node_t {
	void *data;
	struct _node_t *next;
} node_t;

typedef struct _head_t {
	node_t *head;
} head_t;

#define LINK_LIST_NO_FREE_ITEM 0
#define LINK_LIST_FREE_ITEM 1

head_t *link_list_init();
void link_list_delete(head_t *list, uint8_t free_item);

node_t *link_list_append(head_t *list, void *item);
void *link_list_pop(head_t *list);


#endif /* _LINK_LIST_H_ */
