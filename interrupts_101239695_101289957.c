#include "interrupts_101239695_101289957.h"
#define BUFFER_SIZE 32


// instantiating arrays
struct partition partitionArray[6];
struct PCB PCBArray[100];

// =-=     
bool modeBit = 0;
int totalSystemRunTime = 0;
unsigned int cpu_time = 0;
char initName[20];
// initializing vector table
unsigned int vectorTable[] = {0X01E3, 0X029C, 0X0695, 0X042B, 0X0292, 0X048B, 0X0639, 0X00BD, 0X06EF, 0X036C, 0X07B0, 0X01F8, 0X03B9, 0X06C7, 0X0165, 0X0584, 0X02DF, 0X05B3, 0X060A, 0X0765, 0X07B7, 0X0523, 0X03B7, 0X028C, 0X05E8, 0X05D3};

FILE* currentFilePointer;
FILE* externalFilePointer; // create a pointer to the external files file
FILE* outputSecondFilePointer; // create a pointer to the system_status file
FILE* traceFilePointer; // create a pointer to the trace file
FILE* outputFilePointer; // create a pointer to the output file
char buffer[BUFFER_SIZE]; // create a buffer that is capable of reading each line from the trace file


void memorySetup() {
    partitionArray[0].number = 1; partitionArray[0].size = 40; partitionArray[0].occupyingPID = -1;
    partitionArray[1].number = 2; partitionArray[1].size = 25; partitionArray[1].occupyingPID = -1;
    partitionArray[2].number = 3; partitionArray[2].size = 15; partitionArray[2].occupyingPID = -1;
    partitionArray[3].number = 4; partitionArray[3].size = 10; partitionArray[3].occupyingPID = -1;
    partitionArray[4].number = 5; partitionArray[4].size = 8; partitionArray[4].occupyingPID = -1;
    partitionArray[5].number = 6; partitionArray[5].size = 2; partitionArray[5].occupyingPID = -1;
}

void allocateMemory(struct PCB* pcb) {
    for (int i = 0; i < 6; i++ ) {
        if (partitionArray[i].occupyingPID == -1) {
            if (pcb->Mem_Size <= partitionArray[i].size) {
                partitionArray[i].occupyingPID = pcb->PID;
                pcb->partitionInUse = partitionArray[i]->number; 
                pcb->state = READY;
                break;
            }
        }
    }
}

void freeMemory(struct PCB* pcb) {
   partitionArray[pcb->partitionInUse - 1].occupyingPID = -1;
   pcb->partitionInUse = NULL;
}


Boolean programs_done() { //checks if program is done
    for (int i = 0; PCBArray[i] != NULL; i++) {
        if (PCBArray[i].state != TERMINATED) {
            return false;
        }
    }
    return true;
}

Boolean programRunning(){
    for (int i = 0; PCBArray[i] != NULL; i++) {
        if (PCBArray[i].state == RUNNING) {
            return true;
        }
    }

    return false;
}

int readyQueToActivate(ArrayList<int[2]> ready_que) {
    // initialize the earliest index in question and its arrival time from the first index in the ready queue
    int earliest_index = ready_que.get(0).get(0);
    int earliest_time_arrival = ready_que.get(0).get(1);

    // start cycling the ready queue from the second index to the last index
    // to get the earliest arriving program
    for(int i = 1; i< this.ready_que.size(); i++) {
        // if the current program arrives at an earlier time than our previous earliest time
        if (ready_que.get(i).get(1) < earliest_time_arrival) {
            // set earliest index in question and its arrival time from this index in the ready queue
            earliest_index = this.ready_que.get(i).get(0);
            earliest_time_arrival = this.ready_que.get(i).get(1);
        }
    }

    // start cycling the ready queue from the first index to the last index
    // to get the program with the lowest pid of those that qualify as the earliest arriving program
    // ex: multiple programs arrive at 0 ms, we need to choose the one with the lowest pid
    for (int i = 0; i < this.ready_que.size(); i++){
        // if the current program arrives at the same time as our earliest arrival time
        if (ready_que.get(i).get(1) == earliest_time_arrival){
            // if the current program's pid is lower than the program recorded as the earliest to arrive
            if (PCBArray[ready_que.get(i).get(0)].PID < PCBArray[earliest_index].PID){
                // change the earliest arriving program to this one
                earliest_index = ready_que.get(i).get(0);
            }
        }
    }

    // we now have the index of the pcb with the earliest arrival time and the lowest pid
    return ready_que.get(earliest_index).get(0);
}


void FcfsScheduler() {
    ArrayList<int[2]> ready_que; //ready que
    while (!programs_done()) {
        for (int i = 0; PCBArray[i] != NULL; i++) { //this loop checks each PCB in PCB array for if any process has arrived
            
            // checking what's arrived
            if (PCBArray[i].Arrival_Time >= cpu_time && PCBArray[i].state == NEW) { // if PCB has arrived and is not yet assigned

                // look for a partition to assign it to            
                for (int j = 5; j >= 0; j--){
                    // if the current partition is available
                    if (partitionArray[j].occupyingPID == -1){
                        // if the size of the current partition is appropriate
                        if (partitionArray[j].size >= PCBArray[i].Mem_Size){
                            // set partition and pcb information to match each other
                            PCBArray[i].partitionInUse = partitionArray[j].number;
                            partitionArray[j].occupyingPID = PCBArray[i].PID;
                            ready_que.add({i, cpu_time});
                            PCBArray[i].state = READY;
                            break; // break because the search is over
                        }
                    }
                }    
            }


            // maybe
        }

        // check if something is running
        if (programRunning){
            // a program is currently running

        }
        else{
            // a program needs to be assigned
            foreach(int pcbIndex in ready_que)
            {
                // PCBArray[pcbIndex]
            }
        }

        cpu_time++;
    }

}

void PriorityScheduler() {

}


void RoundRobinScheduler() {

}


void routineScheduler(){
    // scheduler called
}


void InputFileProcesser(FILE* traceFilePointer) {
    // 0, 1, 0, 50, 10, 1 instruction parsing
    unsigned int counter = 0;

    while(fgets(buffer, BUFFER_SIZE, traceFilePointer)) {
        char *token = strtok(buffer, ","); // "0"
        PCBArray[counter].PID = atoi(token);
        char *token = strtok(buffer, ","); // "1"
        PCBArray[counter].Mem_Size = atoi(token);
        char *token = strtok(buffer, ","); // "0"
        PCBArray[counter].Arrival_Time = atoi(token);
        char *token = strtok(buffer, ","); // "50"
        PCBArray[counter].CPU_Time = atoi(token);
        char *token = strtok(buffer, ","); // "10"
        PCBArray[counter].IO_Freq = atoi(token);
        char *token = strtok(buffer, ","); // "1"
        PCBArray[counter].IO_Duration = atoi(token);

        counter++;
    }
}

void recordStateTransition(int transiton, Struct PCB* pcb, int oldState, int newState) {

}

void recordMemoryStatus(int time, int memoryUsed, char* partitionState, int totalFree, int usableFree) {

}



int main(int argc, char* argv[])
{
    // initializing partition array and pcb array
    memorySetup();

    // assign MyTraceFile1.txt to traceFilePointer in read mode
    traceFilePointer = fopen(argv[1], "r"); // argv[1]

    // assign ProgramOutput1.txt to outputFilePointer in write mode
    outputFilePointer = fopen(argv[2], "w"); // argv[2]

    // assign system_status.txt to outputSecondFilePointer in write mode
    outputSecondFilePointer = fopen(argv[3], "w"); // argv[3]

    // load PCBs from input file
    InputFileProcesser(traceFilePointer);

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