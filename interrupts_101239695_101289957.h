#ifndef INTERRUPTS_101239695_101289957_H
#define INTERRUPTS_101239695_101289957_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

struct partition {
    int number;
    int size;
    int occupyingPID;
};

typedef struct PCB {
    int PID;
    int CPU_Time;
    int Arrival_Time;
    int Mem_Size;
    int IO_Freq;
    int IO_Duration;
    int Remaining_CPU;
    int partitionInUse;
    enum State {            // Current state of the process
        NEW, READY, RUNNING, WAITING, TERMINATED
    } state;
} pcbShorthand;

typedef struct customQueueNode {
    int index;
    struct PCB* pcb;
    int queueArrivalTime;
    struct customQueueNode* next;
} cqnShorthand;

struct returnPCB {
    struct PCB* pcb;
    struct customQueueNode* cqn;
};

int customQueueLength(struct customQueueNode* headCustomQueueNode);
cqnShorthand *customQueueAddNode(struct customQueueNode* headCustomQueueNode, struct PCB* pcbToAdd, int timeOfArrival);
cqnShorthand *getNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToGetAt);
cqnShorthand *removeNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToRemoveAt);
void memorySetup();
struct PCB *terminateProgram(struct PCB* pcb);
bool programs_done();
struct returnPCB *fcfsSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode);
struct returnPCB *epSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode);
void FcfsScheduler();
void PriorityScheduler();
void RoundRobinScheduler();
void InputFileProcesser(FILE* traceFilePointer);
void recordStateTransition(FILE* execution_file, int timeOfTransition, unsigned int pid, int oldState, int newState);
void recordMemoryStatus(FILE* memory_status_file, int timeOfEvent);

#endif