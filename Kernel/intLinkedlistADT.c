#include "intLinkedlistADT.h"
#include "memoryAllocator.h"
#include "lib.h"

#include <stdint.h>

static intLinkedlistADT remove_int_rec (int num, intLinkedlistADT l);

typedef struct node {
  int elem;
  struct node * next;
} node;

intLinkedlistADT create_list() {
  return NULL;
}

intLinkedlistADT insert_int (intLinkedlistADT first, int num) {
  node * n = (node *) get_page(sizeof(*n));
  n->elem = num;
  n->next = first;
  first = n;

  return n;
}

intLinkedlistADT remove_int (intLinkedlistADT first, int num) {
  return remove_int_rec(num, first);
}

static intLinkedlistADT remove_int_rec (int num, intLinkedlistADT l) {
  if (l == NULL)
    return l;

  if (l->elem == num) {
    node * n = l->next;
    store_page((uint64_t)l);
    return n;
  }

  l->next = remove_int_rec(num, l->next);

  return l;
}

void destroy_int_list(intLinkedlistADT l) {
  node * current = l;
  while (current != NULL) {
    l = current->next;
    store_page((uint64_t)current);
    current = l;
  }
}
