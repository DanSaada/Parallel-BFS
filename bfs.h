#ifndef EX3_BFS_H
#define EX3_BFS_H

#include "SyncGraph/graph.h"
#include "ThreadPool/TaskQueue.h"
#include "ThreadPool/ThreadPool.h"

typedef struct bfsVisitArgs {
    Graph *graph;
    int* distFromStartVertexArr;
    vertex vertexToVisit;
    TaskQueue* oddLevelQ;
    TaskQueue* evenLevelQ;
    ThreadPool* pool;
    int* visited;
    int lvlParityTracker;
} bfsVisitArgs;

typedef struct distanceRowArgs {
    unsigned int numOfVertexes;
    int *distancesFromCurrStartVertexArr;
    vertex currentStartVertex;
    ThreadPool* pool;
} distanceRowArgs;
#endif //EX3_BFS_H
