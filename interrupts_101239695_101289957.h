#ifndef INTERRUPTS_101239695_101289957_H
#define INTERRUPTS_101239695_101289957_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void runCPU(int CPURunTime, FILE* outputToFilePointer);
void runSYSCALL(int vector, unsigned int pcAddress, int ISRBodyRunTime, FILE* outputToFilePointer);
void runENDIO(int vector, unsigned int pcAddress, int ENDIOBodyRunTime, FILE* outputToFilePointer);
void routineScheduler();
void runFORK(int vector, unsigned int pcAddress, int FORKBodyRunTime, FILE* outputToFilePointer, FILE* outputToSecondFilePointer);
void runEXEC(int vector, unsigned int pcAddress, char *theProgramName, int EXECBodyRunTime, FILE* outputToFilePointer, FILE* outputToSecondFilePointer);
void runProgram(FILE* currentProgram);
void printSystemStatus();

#endif