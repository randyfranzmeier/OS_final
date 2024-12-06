#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

//The tree is composed of nodes, which have a reference to the left, right, and some data
//This was made possible with the help of https://www.geeksforgeeks.org/binary-tree-in-c/
typedef struct node {
    struct node *left;
    struct node *right;
    int data;
} node;

//Write and master lock for synchronization
typedef struct sem_lock {
    sem_t criticalLock; // for critical section
    sem_t treeLock; // locks tree
} sem_lock_t;

//The binary search tree itself simply has a head that's a pointer to the head node.
struct BST {
    struct node *head;
} BST_t;

void sem_lock_init(sem_lock_t *lock) {
    sem_init(&lock->criticalLock, 0, 1);
    sem_init(&lock->treeLock, 0, 1);
}

void tree_init(sem_lock_t *lock, int vals[], node *head) {//takes in a list, lock, and bst.
    sem_wait(&lock->criticalLock);
    //approach: go to middle of array each time and search element in middle. That is the element to insert beneath current nodes. head is middle of array
    //testing------------------------------------
    sem_wait(&lock->treeLock);
    &head->data = vals[3];
    printf("Value of head %d", &head->data);
    sem_post(&lock->treeLock);
    //-------------------------------------------
    sem_post(&lock->criticalLock);
}

void tree_insert(sem_lock_t *lock, int value, BST_t *bst) {
    // Acquire the tree lock to ensure exclusive access
    sem_wait(&lock->treeLock);

    // Create a new node for the value
    node *newNode = makeNode(value);
    if (newNode == NULL) {
        perror("Failed to allocate memory for the new node");
        sem_post(&lock->treeLock); // Release the lock
        return;
    }

    // If the tree is empty, set the new node as the head
    if (bst->head == NULL) {
        bst->head = newNode;
        printf("Inserted %d as the head of the tree\n", value);
        sem_post(&lock->treeLock); // Release the lock
        return;
    }

    // Traverse the tree to find the correct position for the new node
    node *current = bst->head;
    node *parent = NULL;

    while (current != NULL) {
        parent = current;
        if (value < current->data) {
            current = current->left;
        } else if (value > current->data) {
            current = current->right;
        } else {
            // Value already exists, do not insert duplicates
            printf("Value %d already exists in the tree\n", value);
            free(newNode); // Free allocated memory for the duplicate node
            sem_post(&lock->treeLock); // Release the lock
            return;
        }
    }

    // Attach the new node to the appropriate parent
    if (value < parent->data) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    printf("Inserted %d into the tree\n", value);

    // Release the lock
    sem_post(&lock->treeLock);
}


void printTree(node *head) {
    //TODO implement this method if time permits
}



int main() {
// binary search tree init
struct BST_t bst;
&bst->head = NULL;
// list of numbers to insert into the tree
int sortedValues[7] = {1,2,3,4,5,8,11};
//lock 
sem_lock_t lock;
sem_lock_init(&lock);

//setting up tree
tree_init(&lock, sortedValues, &bst->head);

return 0;
}
