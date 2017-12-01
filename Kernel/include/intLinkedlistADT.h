#ifndef INT_LINKED_LIST
#define INT_LINKED_LIST

typedef struct node * intLinkedlistADT;

intLinkedlistADT create_list();
intLinkedlistADT insert_int (intLinkedlistADT l, int num);
intLinkedlistADT remove_int (intLinkedlistADT l, int num);
void destroy_int_list(intLinkedlistADT l);

#endif
