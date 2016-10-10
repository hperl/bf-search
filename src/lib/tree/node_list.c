#include "node_list.h"

NodeList *
nl_new()
{
    NodeList *nl = calloc(sizeof(NodeList), 1);
    
    return nl;
}

void
nl_free(NodeList *nl)
{
    for (NodeListItem *item = nl->head; item != NULL && item->next != NULL; item = item->next) {
        free(item);     /* don't free node here! */
    }
    free(nl);
}

void
nl_add_node(NodeList *nl, Node *n)
{
    NodeListItem *new_item = calloc(sizeof(NodeListItem), 1);
    
    new_item->node = n;
    nl->cnt++;
    if (nl_is_empty(nl)) {
        nl->head = new_item;
        nl->tail = new_item;
    } else {
        nl->tail = nl->tail->next = new_item; 
    }
}

void
nl_merge(NodeList *restrict nl1, NodeList *restrict nl2)
{
    if (nl_is_empty(nl1)) {
        /* just copy nodelist */
        memcpy(nl1, nl2, sizeof(NodeList));
    }
    nl1->cnt += nl2->cnt;
    nl1->tail->next = nl2->head;
    nl1->tail = nl2->tail;
    free(nl2);
}

bool
nl_is_empty(NodeList *nl)
{
    return nl->head == NULL;
}

/* iteration */
NodeListItem *
nl_iter_start(NodeList *nl)
{
    return nl->head;
}
