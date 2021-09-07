#include "object/queue.h"

void Queue::enqueue(Chain *item)
{
    item->next = 0;
    *tail = item;
    tail = &(item->next);
}

Chain *Queue::dequeue()
{
  Chain *item;

  item = head;
  if (item) {
    head = item->next;
    if (!head)
      tail = &head;
    else
      item->next = 0;
  }
  return item;
}

void Queue::remove(Chain *item)
{
  Chain *cur;

  if (head) {
    cur = head;
    if (item == cur)
      dequeue();
    else {
      while (cur->next && item != cur->next) {
        cur = cur->next;
      }

      if (cur->next) {
        cur->next = item->next;
        item->next = 0;

        if (!cur->next)
          tail = &(cur->next);
      }
    }
  }
}
