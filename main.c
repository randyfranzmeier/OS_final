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

int treeInitHelper(int vals[], node* cur, int left, int right, int arrLen) {
    //base case
    if (left > right) {
        return 0;
    } 

    int middle = (left + right)/2;
    //invalid middle value
    if (middle >= arrLen) {
        return 0;
    }
    //process left node and children
    if (cur->data > vals[middle]) {
        cur->left = makeNode(vals[middle]);
        return treeInitHelper(vals, cur->left, left, middle-1, arrLen) + treeInitHelper(vals, cur->left, middle+1, right, arrLen);
    }
    //process right node and children
    else {
        cur->right = makeNode(vals[middle]);
        return treeInitHelper(vals, cur->right, middle+1, right, arrLen) + treeInitHelper(vals, cur->right, left, middle-1, arrLen);
    }
}

//This function takes in a sorted list to create a binary search tree by recursively inserting nodes
void tree_init(sem_lock_t *lock, int vals[], BST_t *bst, int arrLen) {//takes in a list, lock, and bst.
    //Binary semaphores for now
    sem_wait(&lock->criticalLock);
    sem_wait(&lock->treeLock);
    //calculate middle to find head node, and keep finding middle recursively
    int left = 0; 
    int right = arrLen;
    int middle = (right + left)/2;
    bst->head = makeNode(vals[middle]);
    //reference to head node
    node* cur = bst->head;
    //left sub tree
    treeInitHelper(vals, cur, left, middle-1, arrLen);
    //right subtree
    treeInitHelper(vals, cur, middle+1, right, arrLen);

    sem_post(&lock->treeLock);
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
int arrLen = (sizeof(sortedValues) / sizeof(sortedValues[0]));
//lock 
sem_lock_t lock;
sem_lock_init(&lock);

//setting up tree
tree_init(&lock, sortedValues, &bst, arrLen);

printf("Head: %d\n", bst.head->data);
printf("left node:%d\nright node: %d\n", bst.head->left->data, bst.head->right->data);

return 0;
}