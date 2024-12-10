//Insertion Test functions

// Structure to pass arguments to threads
typedef struct thread_args
{
    sem_lock_t *lock;
    BST_t *bst;
    char *threadName;
} thread_args_t;

// Thread function to insert random values
void *thread_insert(void *args)
{
    thread_args_t *targs = (thread_args_t *)args;
    sem_lock_t *lock = targs->lock;
    BST_t *bst = targs->bst;
    char *threadName = targs->threadName;

    // Seed the random number generator uniquely for each thread
    srand(time(NULL) ^ pthread_self());

    for (int i = 0; i < 5; i++)
    {
        int value = rand() % 100; // Generate a random number between 0 and 99
        tree_insert(lock, value, bst);
        printf("Thread %s inserted value %d\n", threadName, value);
        sleep(1); // Sleep for 1 second
    }
    return NULL;
}

int main()
{
    // Binary search tree initialization
    BST_t bst;
    bst.head = NULL;

    // List of numbers to insert into the tree
    int sortedValues[7] = {1, 2, 3, 4, 5, 8, 11};
    int arrLen = sizeof(sortedValues) / sizeof(sortedValues[0]);

    // Lock initialization
    sem_lock_t lock;
    sem_lock_init(&lock);

    // Setting up the initial tree
    tree_init(&lock, sortedValues, &bst, arrLen);
    printf("Initialized tree in order: \n");
    printTreeInorder(bst.head, &lock);
    printf("\n");

    // Test deletion of nodes
    testDeleteNodes(&lock, &bst);

    // Prepare thread arguments
    thread_args_t argsA = {&lock, &bst, "A"};
    thread_args_t argsB = {&lock, &bst, "B"};

    // Create threads
    pthread_t threadA, threadB;
    if (pthread_create(&threadA, NULL, thread_insert, &argsA) != 0)
    {
        perror("Failed to create threadA");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&threadB, NULL, thread_insert, &argsB) != 0)
    {
        perror("Failed to create threadB");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    // Print the final tree
    printf("\nFinal tree after insertions:\n");
    printTreeInorder(bst.head, &lock);
    printf("\n");

    // Destroy semaphores
    sem_destroy(&lock.criticalLock);
    sem_destroy(&lock.treeLock);

    return 0;
}