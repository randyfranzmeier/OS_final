#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
// The tree is composed of nodes, which have a reference to the left, right, and some data
// This was made possible with the help of https://www.geeksforgeeks.org/binary-tree-in-c/
#define MAX_THREADS 10
typedef struct node
{
    struct node *left;
    struct node *right;
    int data;
} node;

// Write and master lock for synchronization
typedef struct sem_lock
{
    sem_t criticalLock; // for critical section
    sem_t treeLock;     // locks tree
} sem_lock_t;

// The binary search tree itself simply has a head that's a pointer to the head node.
typedef struct BST
{
    struct node *head;
} BST_t;

// Creates a new binary tree node with the given value, initializes its children to NULL, and returns the created node.
node *makeNode(int value)
{
    // allocate memory for the node
    node *nodeToMake = (node *)malloc(sizeof(node));
    nodeToMake->left = NULL;
    nodeToMake->right = NULL;
    nodeToMake->data = value;
    return nodeToMake;
}

// Initializes the semaphore locks used for synchronizing access to the tree's critical section and overall structure.
void sem_lock_init(sem_lock_t *lock)
{
    sem_init(&lock->criticalLock, 0, 1);
    sem_init(&lock->treeLock, 0, 1);
}

// A helper function to recursively build a binary search tree from a sorted array, inserting nodes into left and right subtrees.
int treeInitHelper(int vals[], node *cur, int left, int right, int arrLen)
{
    // base case
    if (left > right)
    {
        return 0;
    }

    int middle = (left + right) / 2;
    // invalid middle value
    if (middle >= arrLen)
    {
        return 0;
    }
    // process left node and children
    if (cur->data > vals[middle])
    {
        cur->left = makeNode(vals[middle]);
        return treeInitHelper(vals, cur->left, left, middle - 1, arrLen) + treeInitHelper(vals, cur->left, middle + 1, right, arrLen);
    }
    // process right node and children
    else
    {
        cur->right = makeNode(vals[middle]);
        return treeInitHelper(vals, cur->right, middle + 1, right, arrLen) + treeInitHelper(vals, cur->right, left, middle - 1, arrLen);
    }
}

// This function takes in a sorted list to create a binary search tree by recursively inserting nodes
void tree_init(sem_lock_t *lock, int vals[], BST_t *bst, int arrLen)
{ // takes in a list, lock, and bst.
    // Binary semaphores for now
    sem_wait(&lock->criticalLock);
    sem_wait(&lock->treeLock);
    // calculate middle to find head node, and keep finding middle recursively
    int left = 0;
    int right = arrLen;
    int middle = (right + left) / 2;
    bst->head = makeNode(vals[middle]);
    // reference to head node
    node *cur = bst->head;
    // left sub tree
    treeInitHelper(vals, cur, left, middle - 1, arrLen);
    // right subtree
    treeInitHelper(vals, cur, middle + 1, right, arrLen);

    sem_post(&lock->treeLock);
    sem_post(&lock->criticalLock);
}

// Inserts a value into the BST in a thread-safe manner. It ensures no duplicate values are added and locks the tree during the operation.
void tree_insert(sem_lock_t *lock, int value, BST_t *bst)
{
    // Acquire the tree lock to ensure exclusive access
    sem_wait(&lock->treeLock);

    // Create a new node for the value
    node *newNode = makeNode(value);
    if (newNode == NULL)
    {
        perror("Failed to allocate memory for the new node");
        sem_post(&lock->treeLock); // Release the lock
        return;
    }

    // If the tree is empty, set the new node as the head
    if (bst->head == NULL)
    {
        bst->head = newNode;
        printf("Inserted %d as the head of the tree\n", value);
        sem_post(&lock->treeLock); // Release the lock
        return;
    }

    // Traverse the tree to find the correct position for the new node
    node *current = bst->head;
    node *parent = NULL;

    while (current != NULL)
    {
        parent = current;
        if (value < current->data)
        {
            current = current->left;
        }
        else if (value > current->data)
        {
            current = current->right;
        }
        else
        {
            // Value already exists, do not insert duplicates
            printf("Value %d already exists in the tree\n", value);
            free(newNode);             // Free allocated memory for the duplicate node
            sem_post(&lock->treeLock); // Release the lock
            return;
        }
    }

    // Attach the new node to the appropriate parent
    if (value < parent->data)
    {
        parent->left = newNode;
    }
    else
    {
        parent->right = newNode;
    }

    printf("Inserted %d into the tree\n", value);

    // Release the lock
    sem_post(&lock->treeLock);
}

// Finds and returns the parent node of the given value in the tree. Returns NULL if the node has no parent or does not exist.
node *getParent(node *head, int val)
{
    if (head != NULL)
    {
        if (head->data == val)
        {
            return NULL; // no parents
        }
    }

    while (head != NULL)
    {
        if (head->left->data == val || head->right->data == val)
        {
            return head;
        }
        else if (head->data > val)
        {
            head = head->left;
        }
        else if (head->data < val)
        {
            head = head->right;
        }
    }
    // if it reaches here there is no parent
    return NULL;
}

// Finds and returns the in-order successor of a given node, i.e., the smallest node greater than the current node.
node *getSuccessor(node *head, int val)
{
    head = head->right;
    while (head->left != NULL)
    {
        head = head->left;
    }
    return head;
}

// Finds and returns the in-order predecessor of a given node, i.e., the largest node smaller than the current node.
node *getPredecessor(node *head, int val)
{
    head = head->left;
    while (head->right != NULL)
    {
        head = head->right;
    }
    return head;
}

// Deletes a node with the specified value from the BST in a thread-safe manner. Handles different cases of node deletion: leaf node, single child, or two children.
void deleteVal(sem_lock_t *lock, int value,BST_t *bst)
{
    // acquire lock
    sem_wait(&lock->criticalLock);
    sem_wait(&lock->treeLock);
    node *current = bst->head;
    if (current->data == value)
    {
        // root node deletion
        node *succOrPred = getSuccessor(bst->head, value);
        if (succOrPred == NULL)
        {
            succOrPred = getPredecessor(bst->head, value);
            if (succOrPred == NULL)
            {
                printf("deleting whole tree\n");
                bst->head = NULL;
                sem_post(&lock->criticalLock);
                sem_post(&lock->treeLock);
                return;
            }
        }
        printf("deleting root node\n");
        int temp = succOrPred->data;
        node *parent = getParent(bst->head, temp);
        if (parent->data > value)
        {
            parent->left = NULL;
        }
        else
        {
            parent->right = NULL;
        }
        current->data = temp;
        sem_post(&lock->criticalLock);
        sem_post(&lock->treeLock);
        return;
    }
    while (current != NULL)
    {
        // move left or right depending
        // on value >< data
        if (value > current->data)
        {
            current = current->right;
        }
        else if (value < current->data)
        {
            current = current->left;
        }
        // if neither condition is met that means
        // that the value of the current node is the
        // value of what you are looking for
        else
        {
            printf("\ndeleting %d\n", current->data);
            // leaf node
            if (current->left == NULL && current->right == NULL)
            {
                node *parent = getParent(bst->head, value);
                if (parent->data > value)
                {
                    parent->left = NULL;
                }
                else
                {
                    parent->right = NULL;
                }
            }
            // single right child
            else if (current->left == NULL)
            {
                node *parent = getParent(bst->head, value);
                if (parent->data > value)
                {
                    parent->left = current->right;
                }
                else
                {
                    parent->right = current->right;
                }
            }
            // single left child
            else if (current->right == NULL)
            {
                node *parent = getParent(bst->head, value);
                if (parent->data > value)
                {
                    parent->left = current->left;
                }
                else
                {
                    parent->right = current->left;
                }
            }
            // 2 children
            else
            {
                node *succ = getSuccessor(current, value);
                int temp = succ->data;
                node *parent = getParent(bst->head, temp);
                if (parent->data > value)
                {
                    parent->left = NULL;
                }
                else
                {
                    parent->right = NULL;
                }
                current->data = temp;
            }
            sem_post(&lock->criticalLock);
            sem_post(&lock->treeLock);
            return;
        }
    }
    printf("Unable to locate: %d in tree\n", value);
    sem_post(&lock->criticalLock);
    sem_post(&lock->treeLock);
    return;
}

// Performs an in-order traversal of the BST and prints each node's value. Locks are used to ensure thread-safe printing.
void printTreeInorder(node *head, sem_lock_t *lock)
{
    // notedly the method I converted over from python
    // actually has more lines of code
    // if the head is NULL, you've reached the end of the tree
    if (head == NULL)
    {
        return;
    }
    // if not then go all the way left
    printTreeInorder(head->left, lock);
    // once you've gone all the way left print the node
    sem_wait(&lock->criticalLock);
    printf("%d ", head->data);
    sem_post(&lock->criticalLock);
    // then go alllll the way right
    printTreeInorder(head->right, lock);
}

// Searches for a specified value in the BST and returns 1 if found, 0 otherwise. Prints the result of the search operation.
int searchTree(node *head, int value)
{
    // loop down node path until you reach null
    while (head != NULL)
    {
        // move left or right depending
        // on value >< data
        if (value > head->data)
        {
            head = head->right;
        }
        else if (value < head->data)
        {
            head = head->left;
        }
        // if neither condition is met that means
        // that the value of the current node is the
        // value of what you are looking for
        else
        {
            printf("\nFound: %d\n", value);

            return 1;
        }
    }
    // if you reached null then the value is not in
    // tree
    printf("\nUnable to locate: %d in tree\n", value);
    return 0;
}

// A test function that deletes specific nodes (2 and 4) from the BST and prints the tree's state after each deletion.
void testDeleteNodes(sem_lock_t *lock, BST_t *bst)
{
    deleteVal(lock, rand() % MAX_THREADS, bst);
    printTreeInorder(bst->head, lock);
    printf("\n");
    printf("\n");
}
typedef struct{
    int thread_id;
    BST_t bst;
    sem_lock_t lock;
    int value;
    void(*func)(sem_lock_t *,int,BST_t *);
}thread_data_t;

void *thread_gen(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;
	//printf("Hello from thread %d\n", data->value);
    
	data->func(&data->lock,data->value,&data->bst);  // Call the function passed
	pthread_exit(NULL);
}



int main()
{
    // binary search tree init
    BST_t bst;
    bst.head = NULL;

    // list of numbers to insert into the tree
    int sortedValues[7] = {1, 2, 3, 4, 5, 8, 11};
    int arrLen = (sizeof(sortedValues) / sizeof(sortedValues[0]));
    // lock
    sem_lock_t lock;
    sem_lock_init(&lock);
    // setting up tree
    tree_init(&lock, sortedValues, &bst, arrLen);
    printf("Initialized tree in order: \n");

    int num_threads;
    int done = 0;
    thread_data_t td[MAX_THREADS];
    
    
    while (done == 0){
        printf("Enter a number of threads (10 or fewer): ");
        if (scanf("%d", &num_threads)!=1){
            printf("\nInvalid input. Enter a valid number.\n");
            while (getchar()!= '\n');
            continue;

        }
        if (num_threads <= MAX_THREADS && num_threads > 0) {
			done = 1;
		} else {
			printf("\nEnter a number less than 10.\n");
		}

    }
    pthread_t threads[MAX_THREADS];
    for (int i = 0; i<num_threads;i++){
        td[i].thread_id;
        td[i].lock = lock;
        td[i].bst = bst;
        td[i].value =  rand() % 100;
        td[i].func = tree_insert;
        if (pthread_create(&threads[i], NULL, thread_gen, &td[i]) != 0) {
			perror("pthread_create failed");
			exit(EXIT_FAILURE);
		}
        testDeleteNodes(&lock, &bst);
    }
    
    //tree_insert(&lock,15,&bst);
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Initialized tree in order: \n");
    printTreeInorder(bst.head, &lock);

    return 0;
}
