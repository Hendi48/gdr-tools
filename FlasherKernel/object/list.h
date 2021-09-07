#pragma once

#include "object/queue.h"

class List : public Queue
{
private:
    List (const List &copy);

public:
    List () {}

    // Returns the first element of the list
    Chain* first () { return head; }

    // Inserts the element at the beginning of the list
    void insert_first (Chain* new_item);

    // Inserts the new element after the specified old element
    void insert_after (Chain* old_item, Chain* new_item);
};
