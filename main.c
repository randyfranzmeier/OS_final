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
typedef struct BST {
    struct node *head;
} BST_t;

node* makeNode(int value) {
    //allocate memory for the node
    node* nodeToMake = (node*)malloc(sizeof(node));
    nodeToMake->left = NULL;
    nodeToMake->right = NULL;
    nodeToMake->data = value;
    return nodeToMake;
}

void sem_lock_init(sem_lock_t *lock) {
    sem_init(&lock->criticalLock, 0, 1);
    sem_init(&lock->treeLock, 0, 1);
}

void tree_init(sem_lock_t *lock, int vals[], BST_t *bst) {//takes in a list, lock, and bst.
    sem_wait(&lock->criticalLock);
    //approach: go to middle of array each time and search element in middle. That is the element to insert beneath current nodes. head is middle of array
    //testing------------------------------------
    sem_wait(&lock->treeLock);
    bst->head = makeNode(vals[3]);
    printf("Value of head: %d\n", bst->head->data);
    sem_post(&lock->treeLock);
    //-------------------------------------------
    sem_post(&lock->criticalLock);
}

void printTree(node *head) {
    //TODO implement this method if time permits
}



int main() {
// binary search tree init
BST_t bst;
bst.head = NULL;
// list of numbers to insert into the tree
int sortedValues[7] = {1,2,3,4,5,8,11};
//lock 
sem_lock_t lock;
sem_lock_init(&lock);

//setting up tree
tree_init(&lock, sortedValues, &bst);

return 0;
}
