// A complete working C++ program to demonstrate
//  all insertion methods on Linked List
#include <iostream>
using namespace std;

// A linked list node

struct Node
{
    int data;
    Node* next;
};

Node* newNode(const int value)
{
    Node* n_node = new Node();
    n_node->data = value;
    n_node->next = nullptr;

    return n_node;
}
struct List {
    Node* first; // adresa prveho uzla zoznamu
};

/* Given a reference (pointer to pointer)
to the head of a list and an int, inserts
a new node on the front of the list. */
void push(Node** head_ref, int new_data)
{
    /* 1. allocate node */
    Node* new_node = newNode(new_data);

    /* 2. put in the data */

    /* 3. Make next of new node as head */
    new_node->next = (*head_ref);

    /* 4. move the head to point to the new node */
    (*head_ref) = new_node;
}

/* Given a node prev_node, insert a new node after the given
prev_node */
void insertAfter(Node* prev_node, int new_data)
{
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL)
    {
        cout << "the given previous node cannot be NULL";
        return;
    }

    /* 2. allocate new node */
    Node* new_node = newNode(new_data);

    /* 3. put in the data */

    /* 4. Make next of new node as next of prev_node */

    /* 5. move the next of prev_node as new_node */
    prev_node->next = new_node;
}

/* Given a reference (pointer to pointer) to the head
of a list and an int, appends a new node at the end */
void append(Node** head_ref, int new_data)
{
    /* 1. allocate node */
    Node* new_node = newNode(new_data);

    Node* last = *head_ref; /* used in step 5*/

    /* 4. If the Linked List is empty,
    then make the new node as head */
    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = new_node;
    return;
}

// This function prints contents of
// linked list starting from head
void printList(List* node)
{   
    Node* tmp = node->first;
    while (tmp != NULL)
    {
        cout << " " << tmp->data;
        tmp = tmp->next;
    }
}



/* Driver code*/
int main()
{
    /* Start with the empty list */
    Node* head = NULL;
    List* list=new List();
    list->first=head;

    // Insert 6. So linked list becomes 6->NULL
    append(&list->first, 6);

    // Insert 7 at the beginning.
    // So linked list becomes 7->6->NULL
    push(&list->first, 7);

    // Insert 1 at the beginning.
    // So linked list becomes 1->7->6->NULL
    push(&list->first, 1);

    // Insert 4 at the end. So
    // linked list becomes 1->7->6->4->NULL
    append(&list->first, 4);

    // Insert 8, after 7. So linked
    // list becomes 1->7->8->6->4->NULL
   // insertAfter(&list->first->next, 8);

    cout << "Created Linked list is: ";
    printList(list);

    return 0;
}

