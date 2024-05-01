#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bfs.h"
#include "SyncGraph/graph.h"
#include "ThreadPool/TaskQueue.h"
#include "ThreadPool/ThreadPool.h"
#include <limits.h>  // For INT_MAX

#define INFINITY (-1)
#define MAX_THREAD_NUMBER (4)
#define EVEN_LEVEL (0)
#define VISITED (1)
#define TOGGLE_LEVEL_PARITY(levelParityTracker)  (((levelParityTracker) + 1) % 2)




void* replaceInfintyRepresetionInRow(void* args){
    distanceRowArgs* data = (distanceRowArgs*) args;
    unsigned int numOfVertexes = data->numOfVertexes;
    int* distArr = data->distancesFromCurrStartVertexArr;
    vertex v = data->currentStartVertex;
    ThreadPool *pool = data->pool;
    for (unsigned int i = 0; i < numOfVertexes; ++i){
        if (distArr[i] == INT_MAX)
            distArr[i] = INFINITY;
    }
    free(data);
//    decreaseRunningThreadCount(pool);
    --(pool->runningThreads);
//    atomic_fetch_sub(&(pool->runningThreads), 1);
    return NULL;
}

void* setInitialDist(void *args){
    distanceRowArgs* data = (distanceRowArgs*) args;
    unsigned int numOfVertexes = data->numOfVertexes;
    int* distArr = data->distancesFromCurrStartVertexArr;
    vertex v = data->currentStartVertex;
    ThreadPool *pool = data->pool;
    for (unsigned int i = 0; i < numOfVertexes; ++i){
        distArr[i] = (i == v) ? 0 : INT_MAX;
    }
    free(data);
//    decreaseRunningThreadCount(pool);
    --(pool->runningThreads);
//    atomic_fetch_sub(&(pool->runningThreads), 1);
    return NULL;
}

void setDistMatrix(int **m, unsigned int numVertices, void* (*rowSettingFunc)(void*)) {
    // Initialize the matrix m with INT_MAX to represent infinite distance initially
    unsigned int i = 0;
    TaskQueue settingRowsTaskQ;
    initQueue(&settingRowsTaskQ);
    ThreadPool pool;
    pool.maxThreads = MAX_THREAD_NUMBER;
    pool.runningThreads = 0;
    pool.q = &settingRowsTaskQ;
    for (; i < numVertices; ++i) {
        distanceRowArgs *params = (distanceRowArgs*) malloc(sizeof(distanceRowArgs));   // freed at setInitialDist
        if (params == NULL){
            printf("Memory allocation failed!\n");
            exit(1);
        }
        params->numOfVertexes = numVertices;
        params->currentStartVertex = i;         // check the narrowing subject out
        params->distancesFromCurrStartVertexArr = m[i];
        params->pool = &pool;
        TaskData setRowTaskData = {rowSettingFunc, params};
        insert(&settingRowsTaskQ, setRowTaskData);
    }
    runThreadPool(&pool);
}

void * bfsVisit(void *args){
    bfsVisitArgs *data = (bfsVisitArgs *) args;
    Graph *graph = data->graph;
    int* distArr = data->distFromStartVertexArr;
    vertex currVisitedVertex = data->vertexToVisit;
    TaskQueue* evenLevelQ = data->evenLevelQ;
    TaskQueue* oddLevelQ = data->oddLevelQ;
    ThreadPool* pool = data->pool;
    int* visited = data->visited;
    int lvlParityTracker = data->lvlParityTracker;

    node* neighborsPtr = graph->adjacencyLists[currVisitedVertex];
    while (neighborsPtr != NULL){
        vertex neighbor = neighborsPtr->v;
        pthread_mutex_lock(&graph->num_visits_mutexes[neighbor]);   // not sure about this...
        if(!visited[neighbor]){
            distArr[neighbor] = distArr[currVisitedVertex] + 1;
            visited[neighbor] = VISITED;

            bfsVisitArgs *params = malloc(sizeof(bfsVisitArgs));
            params->graph = graph;
            params->evenLevelQ = evenLevelQ ;
            params->oddLevelQ = oddLevelQ;
            params->vertexToVisit = neighbor;
            params->distFromStartVertexArr = distArr;
            params->pool = pool;
            params->visited = visited;
            params->lvlParityTracker =  TOGGLE_LEVEL_PARITY(lvlParityTracker);
            TaskData td = {bfsVisit, params};
            if (lvlParityTracker == EVEN_LEVEL){    // if this is an ODD number lvl then next lvl number should be even
                insert(oddLevelQ,td);
            } else {        // if this is an EVEN number lvl then next lvl number should be odd
                insert(evenLevelQ,td);
            }
        }
//        pthread_mutex_unlock(&graph->num_visits_mutexes[currVisitedVertex]);
        pthread_mutex_unlock(&graph->num_visits_mutexes[neighbor]);
        neighborsPtr = neighborsPtr->next;
    }
    --(pool->runningThreads);
//    atomic_fetch_sub(&(pool->runningThreads), 1);
    free(data);
    return NULL;
}

void realBfs(Graph *graph, int *distsFromStartVert, vertex startVertex){
    distsFromStartVert[startVertex] = 0;    // distance from vertex to itself is zero
    int *visited = (int *)calloc(graph->numVertices, sizeof(int));
    visited[startVertex] = 1;
    TaskQueue evenLevelQ;
    initQueue(&evenLevelQ);
    TaskQueue oddLevelQ;
    initQueue(&oddLevelQ);

    ThreadPool pool;
    pool.maxThreads = MAX_THREAD_NUMBER;
    pool.runningThreads = 0;
    pool.q = &evenLevelQ;

    int levelParityTracker = 0;
    bfsVisitArgs *params = malloc(sizeof(bfsVisitArgs));
    params->graph = graph;
    params->evenLevelQ = &evenLevelQ;
    params->oddLevelQ = &oddLevelQ;
    params->vertexToVisit = startVertex;
    params->distFromStartVertexArr = distsFromStartVert;
    params->pool = &pool;
    params->visited = visited;
    params->lvlParityTracker = levelParityTracker;
    TaskData td = {bfsVisit, params};
    insert(&evenLevelQ,td);
    do {
        runThreadPool(&pool);
        levelParityTracker = TOGGLE_LEVEL_PARITY(levelParityTracker);
        if (levelParityTracker == EVEN_LEVEL){
            pool.q = &evenLevelQ;
        } else {
            pool.q = &oddLevelQ;
        }
    } while (!isEmpty(pool.q));
    free(visited);
}


void bfs(Graph *graph, int **m) {
    unsigned int numVertices = graph->numVertices;
    setDistMatrix(m, numVertices, setInitialDist);
    for (vertex v = 0; v < numVertices; v++) {
        realBfs(graph,m[v], v);
    }

    // resetting infinite values as requested
    setDistMatrix(m, numVertices, replaceInfintyRepresetionInRow);
}