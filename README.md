# OS_final: Thread-safe Binary Search Tree

## Authors
Ian McNiesh, Zachary Stefanich, Randy Franzmeier

## Overview
This project deals with synchronizing a binary search tree being implemented in a multithreaded environment. In addition to having the tree, we will have important functions like insert, delete, and add, as well as tests to assure it's operating as expected.

## Sub-Tasks
1. Implement the Base Binary Search Tree
    Develop the foundational structure and functionality of the binary tree in C.
    Use the GeeksforGeeks implementation as a reference point for initial development. *note I am not yet testing here. Just writing the tree and some functions
    Implement basic operations, including insertion, deletion, and traversal, to ensure the binary tree works correctly as a single-threaded structure.
2. Build Test Cases for Creation, Insertion, Deletion, and Searching
    Develop a comprehensive suite of test cases to validate the binary tree’s functionality under various scenarios.
    Include edge cases such as inserting duplicate values, handling large datasets, and removing nodes with zero, one, or two children.
    Create structured tests to ensure the binary tree is created properly and remains valid across operations.
    Implement tests for searching functionality, verifying correct results for present and absent elements, as well as minimal node traversal
    Use automated testing tools or frameworks to streamline the testing process and identify potential bugs early in development.
    Figure out ways to work around duplication, as discussed in this stack overflow post
3. Implement Multi-Threaded Usage
    Introduce threading to allow simultaneous operations on the binary tree, such as concurrent insertions, deletions, and searches.
    Use C’s threading libraries (like Posix threads) to create threads that interact with the binary tree in real-time.
    Ensure threads operate independently while adhering to synchronization protocols, minimizing contention and bottlenecks.
    Simulate realistic use cases, such as a database system where multiple users perform concurrent read and write operations.
    Analyze the implementation for thread safety and performance, making adjustments to handle conflicts and optimize efficiency.
4. Performance: Multi-Threaded vs. Single-Threaded Binary Trees
    Conduct performance benchmarking to compare the processing speed of the thread-safe binary tree against a standard single-threaded implementation.
    Measure the time taken for operations such as insertion, deletion, and searching under varying workloads, including small, medium, and large datasets.
    Simulate different levels of concurrency by varying the number of threads performing operations simultaneously.
    Analyze the results to evaluate the impact of synchronization mechanisms on performance and identify potential bottlenecks.
    Document findings with graphs and detailed analysis to provide clear insights into the efficiency trade-offs between the two implementations. 
5. Build Test Cases for Threaded Version
    Design a robust set of test cases specifically tailored to the multi-threaded binary tree.
    Test concurrent insertion, deletion, and searching under normal and edge-case conditions, such as high contention scenarios.
    Verify the correctness of operations by ensuring data integrity, consistency, and proper handling of simultaneous modifications.
    Use tools like Helgrind or Valgrind to detect and prove the absence of race conditions, deadlocks, and other concurrency-related issues.
    Develop stress tests to push the threaded implementation to its limits, confirming its stability and reliability under heavy load.
