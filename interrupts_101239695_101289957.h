#ifndef INTERRUPTS_101239695_101289957_H
#define INTERRUPTS_101239695_101289957_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

struct partition {
    unsigned int number;
    unsigned int size;
    int occupyingPID;
};

typedef struct PCB {
    unsigned int PID;
    unsigned int CPU_Time;
    unsigned int Arrival_Time;
    unsigned int Mem_Size;
    unsigned int IO_Freq;
    unsigned int IO_Duration;
    unsigned int Remaining_CPU;
    unsigned int partitionInUse;
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

int customQueueLength(struct customQueueNode* headCustomQueueNode);
void customQueueAddNode(struct customQueueNode* headCustomQueueNode, struct PCB* pcbToAdd, int timeOfArrival);
cqnShorthand *getNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToGetAt);
int removeNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToRemoveAt);
void memorySetup();
void terminateProgram(struct PCB* pcb);
bool programs_done();
pcbShorthand *fcfsSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode);
pcbShorthand *epSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode);
void FcfsScheduler();
void PriorityScheduler();
void RoundRobinScheduler();
void InputFileProcesser(FILE* traceFilePointer);
void recordStateTransition(FILE* execution_file, int timeOfTransition, unsigned int pid, int oldState, int newState);
void recordMemoryStatus(FILE* memory_status_file, int timeOfEvent);

#endif