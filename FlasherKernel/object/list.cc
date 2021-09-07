#include "object/list.h"

void List::insert_first (Chain* new_item)
{
    if (head) {
        new_item->next = head;
        head = new_item;
    } else {
        enqueue (new_item);
    }
}

void List::insert_after (Chain* old_item, Chain* new_item)
{
    new_item->next = old_item->next;
    old_item->next = new_item;
}
