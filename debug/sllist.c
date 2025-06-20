/****************************************************************************
 * Copyright © 2024 Rose-Hulman Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ****************************************************************************/

#include "sllist.h"

void init_node(struct list_node *node)
{
  // circular linked list, so next points to self when empty or not in use.
  node->next = node;
}

// add at head
void add_head(struct list_node *head, struct list_node *node)
{
  node->next = head->next;
  node->next = head->next;
  head->next = node;
}

// add at tail
void add_tail(struct list_node *head, struct list_node *node)
{
  struct list_node *p = head->next;

  while(p->next != head) {
    p = p->next;
  }

  node->next = p->next;
  node->next = p->next;
  p->next = node;
}

// delete a node
void del_node(struct list_node *head, struct list_node *node)
{
  struct list_node *p = head, *n = head->next;

  // find the node
  while(n != node && n != head) {
    p = n;
    n = n->next;
  }

  // empty!
  if(n == head) return;

  p->next = n->next;
  n->next = n;
}

// print the list
void print_list(struct list_node *head, void (*print_fn)(struct list_node *))
{
  struct list_node *p = head->next;

  while(p != head) {
    print_fn(p);
    p = p->next;
  }
}

