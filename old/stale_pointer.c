// stale_pointer.c
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int value;
    struct Node *next;
};

int main(void) {
    struct Node *n1 = malloc(sizeof(struct Node));
    struct Node *n2 = malloc(sizeof(struct Node));
    if (!n1 || !n2) return 1;

    n1->value = 1;
    n1->next = n2;
    n2->value = 2;
    n2->next = NULL;

    struct Node *head = n1;
    struct Node *saved_next = head->next; // old pointer (A)

    free(n2);                             // free node A
    struct Node *n3 = malloc(sizeof(struct Node)); // maybe reuses memory
    n3->value = 3;
    n3->next = NULL;
    head->next = n3;                      // list is now 1 -> 3

    // BUG: use stale pointer that may now point to new object
    printf("Stale pointer value (UB): %d\n", saved_next->value);

    free(n1);
    free(n3);
    return 0;
}
