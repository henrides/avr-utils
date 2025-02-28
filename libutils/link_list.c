#include "link_list.h"

#include <stdlib.h>

head_t *link_list_init()
{
	head_t *list = (head_t *)malloc(sizeof(head_t));
	list->head = NULL;
	return list;
}

void link_list_delete(head_t *list, uint8_t free_item)
{
	node_t *i;
	for (i = list->head; i != NULL;)
	{
		node_t *next = i->next;
		if (free_item) {
			free(i->data);
		}
		free(i);
		i = next;
	}

	free(list);
}

node_t *link_list_append(head_t *list, void *item)
{
	node_t *node = (node_t *)malloc(sizeof(node_t));
	node->data = item;
	node->next = NULL;

	if (!list->head) {
		list->head = node;
	} else {
		node_t *i;
		for (i = list->head; i != NULL; i = i->next) {
			if (!i->next) {
				i->next = node;
				break;
			}
		}
	}
	return node;
}

void *link_list_pop(head_t *list)
{
    void *item = NULL;
    if (list->head) {
        item = list->head->data;
        node_t *del = list->head;
        list->head = list->head->next;
        free(del);
    }
    return item;
}
