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
void deleteVal(sem_lock_t *lock,node* head,int value){
    //acquire lock
    sem_wait(&lock->treeLock);
    node* current = head;
    while(head != NULL){
        //move left or right depending 
        //on value >< data
        if (value>head->data){
            head = head->right;
        }
        else if (value<head->data)
        {
            head = head-> left;
        }
        //if neither condition is met that means
        //that the value of the current node is the
        //value of what you are looking for
        else
        {   
            
            //needs delete logic somewhere around here
            sem_post(&lock->treeLock);
            return;
        }

        
    }
    printf("Unable to locate: %d in tree",value);
    sem_post(&lock->treeLock);
    return;
    
}
void printTree(node* head) {
    //notedly the method I converted over from python 
    //actually has more lines of code
    //if the head is NULL, you've reached the end of the tree
    if (head == NULL){
        return;
    }
    //if not then go all the way left
    printTree(head->left);
    //once you've gone all the way left print the node
    printf("%d ", head->data);
    //then go alllll the way right
    printTree(head->right);
}
int searchTree(node* head,int value){
    //loop down node path until you reach null
    while(head != NULL){
        //move left or right depending 
        //on value >< data
        if (value>head->data){
            head = head->right;
        }
        else if (value<head->data)
        {
            head = head-> left;
        }
        //if neither condition is met that means
        //that the value of the current node is the
        //value of what you are looking for
        else
        {
            printf("/nFound: %d",value);

            return 1;
        }
        
    }
    //if you reached null then the value is not in
    //tree
    printf("Unable to locate: %d in tree",value);
    return 0;
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
//printTree(bst.head);
//searchTree(bst.head,12);
deleteVal(&lock,bst.head,3);
printTree(bst.head);

return 0;
}
