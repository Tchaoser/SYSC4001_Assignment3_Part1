#include "interrupts_101239695_101289957.h"
#define BUFFER_SIZE 32

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

// instantiating arrays
struct partition partitionArray[6];
struct PCB PCBArray[100];

// =-=     
bool modeBit = 0;
int totalSystemRunTime = 0;
char initName[20];
// initializing vector table
unsigned int vectorTable[] = {0X01E3, 0X029C, 0X0695, 0X042B, 0X0292, 0X048B, 0X0639, 0X00BD, 0X06EF, 0X036C, 0X07B0, 0X01F8, 0X03B9, 0X06C7, 0X0165, 0X0584, 0X02DF, 0X05B3, 0X060A, 0X0765, 0X07B7, 0X0523, 0X03B7, 0X028C, 0X05E8, 0X05D3};

FILE* currentFilePointer;
FILE* externalFilePointer; // create a pointer to the external files file
FILE* outputSecondFilePointer; // create a pointer to the system_status file
FILE* traceFilePointer; // create a pointer to the trace file
FILE* outputFilePointer; // create a pointer to the output file
char buffer[BUFFER_SIZE]; // create a buffer that is capable of reading each line from the trace file

void routineScheduler(){
    // scheduler called
}

int main(int argc, char* argv[])
{
    // initializing pcb array
    // todo

    // assign MyTraceFile1.txt to traceFilePointer in read mode
    traceFilePointer = fopen(argv[1], "r"); // argv[1]

    // assign ProgramOutput1.txt to outputFilePointer in write mode
    outputFilePointer = fopen(argv[2], "w"); // argv[2]

    // assign system_status.txt to outputSecondFilePointer in write mode
    outputSecondFilePointer = fopen(argv[3], "w"); // argv[3]

    // initializing partition array
    partitionArray[0].number = 1; partitionArray[0].size = 40; partitionArray[0].occupyingPID = -1;
    partitionArray[1].number = 2; partitionArray[1].size = 25; partitionArray[1].occupyingPID = -1;
    partitionArray[2].number = 3; partitionArray[2].size = 15; partitionArray[2].occupyingPID = -1;
    partitionArray[3].number = 4; partitionArray[3].size = 10; partitionArray[3].occupyingPID = -1;
    partitionArray[4].number = 5; partitionArray[4].size = 8; partitionArray[4].occupyingPID = -1;
    partitionArray[5].number = 6; partitionArray[5].size = 2; // strcpy(partitionArray[5].code, argv[1]); // argv[1]

    // system_status initial print
    //printSystemStatus();

    // runProgram(traceFilePointer);

    // End of instructions, close program

    fclose(traceFilePointer);
    fclose(outputFilePointer);
    fclose(outputSecondFilePointer);
    return 0;   

    /*
    // randomly divide EXECBodyRunTime between the 5 tasks in EXEC body
    int firstDivision = EXECBodyRunTime/5;
    srand(time(0));
    int rando = (rand() % firstDivision) + 1;
    int Time1 = firstDivision + rando; // create 1st time block
    int secondDivision = (EXECBodyRunTime - Time1) / 4;
    rando = (rand() % secondDivision) + 1;
    int Time2 = secondDivision + rando; // create 2nd time block
    int thirdDivision = (EXECBodyRunTime - Time1 - Time2) / 3;
    rando = (rand() % thirdDivision) + 1;
    int Time3 = thirdDivision + rando; // create 3rd time block
    int fourthDivision = (EXECBodyRunTime - Time1 - Time2 - Time3) / 2;
    rando = (rand() % fourthDivision) + 1;
    int Time4 = fourthDivision + rando; // create fourth time block
    int Time5 = EXECBodyRunTime - Time1 - Time2 - Time3 - Time4; // create fifth time block
    */
}