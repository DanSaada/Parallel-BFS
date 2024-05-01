# Parallel-BFS
This project implement a parallel BFS algorithm for undirected graphs, which calculates the shortest path matrix for a given graph.

![image](https://github.com/DanSaada/Parallel-BFS/assets/112869076/f5bc4f0f-0adf-4d02-9cd3-b7c969923b89)

## Implementation
* Graph: Handles the representation and initialization of the graph.
* TaskQueue: Manages the queue of tasks (nodes to be processed) in a thread-safe manner.
* ThreadPool: Manages worker threads that execute BFS concurrently.
* BFS: Contains the BFS algorithm logic that interacts with other components to perform parallel traversal.

## Installing And Executing
    
To clone and run this application, you'll need [Git](https://git-scm.com) installed on your computer.
  
From your command line:

  
```bash
# Clone this repository.
$ git clone https://github.com/DanSaada/Parallel-BFS.git

# Go into the repository.
$ cd Parallel-BFS

# Compile using the makefile.
$ make

# Run the program.
 ./parallelBFS.out
```

## Author
- [Dan Saada](https://github.com/DanSaada)

