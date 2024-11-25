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

struct PCB {
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
};

struct readyQueNode{
    struct PCB* pcb
    readyQueNode* next;
}


void routineScheduler();
void memorySetup();
void InputFileProcesser(FILE* traceFilePointer);
void FcfsScheduler();
void PriorityScheduler();
void RoundRobinScheduler();
void allocateMemory(struct PCB* pcb);
void freeMemory(struct PCB* pcb);
void recordStateTransition(int transition, Struct PCB* pcb, int oldState, int newState);
void recordMemoryStatus(int time, int memoryUsed, char* partitionState, int totalFree, int usableFree);

#endif