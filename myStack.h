#include <stdio.h>
#include <stdlib.h>

struct Node
{
    char *data;
    struct Node *next;
};
Node *head = NULL;

char *top()
{
    if (head){return head->data;}
    return NULL;
}
char *pop()
{
    if (!head){return NULL;}
    struct Node *old_head = head;
    char *old_data = head->data;
    head = head->next;
    free(old_head);
    return old_data;
}
void push(char *input)
{
    struct Node *newNode;
    newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = input;
    if (head){newNode->next = head;}
    else{newNode->next = head;}
    head = newNode;
}