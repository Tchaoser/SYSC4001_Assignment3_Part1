#include "interrupts_101239695_101289957.h"
#define BUFFER_SIZE 32

// instantiating arrays
struct partition partitionArray[6];
struct PCB PCBArray[100];

// =-=     
bool modeBit = 0;
int totalSystemRunTime = 0;
unsigned int cpu_time = 0;
// initializing vector table
unsigned int vectorTable[] = {0X01E3, 0X029C, 0X0695, 0X042B, 0X0292, 0X048B, 0X0639, 0X00BD, 0X06EF, 0X036C, 0X07B0, 0X01F8, 0X03B9, 0X06C7, 0X0165, 0X0584, 0X02DF, 0X05B3, 0X060A, 0X0765, 0X07B7, 0X0523, 0X03B7, 0X028C, 0X05E8, 0X05D3};

FILE* currentFilePointer;
FILE* externalFilePointer; // create a pointer to the external files file
FILE* outputSecondFilePointer; // create a pointer to the system_status file
FILE* traceFilePointer; // create a pointer to the trace file
FILE* outputFilePointer; // create a pointer to the output file
char buffer[BUFFER_SIZE]; // create a buffer that is capable of reading each line from the trace file

// customQueue operations
int customQueueLength(customQueueNode* headCustomQueueNode){
    int lengthCounter = 0;
    customQueueNode* current_node = headCustomQueueNode;
    for (; current_node != NULL; current_node = current_node->next){
        lengthCounter++
    }

    return lengthCounter++;
}

// Add a node to the end of the ready queue with its relevant information
void customQueueAddNode(struct customQueueNode* headCustomQueueNode, struct PCB* pcbToAdd, int timeOfArrival){
    struct customQueueNode* nodeToAdd = malloc(sizeof(struct customQueueNode));
    int queueLength = readyQueueLength(headReadyQueueNode);
    nodeToAdd->index = queueLength;
    nodeToAdd->pcb = pcbToAdd;
    nodeToAdd->queueArrivalTime = timeOfArrival;
    nodeToAdd->next = NULL;

    if (queueLength == 0){
        headCustomQueueNode = nodeToAdd;
        return;
    }
    else{
        customQueueNode* current_node = headCustomQueueNode;
        // Find the last node and set its next node to the nodeToAdd;
        for (; current_node != NULL; current_node = current_node->next){
            if (current_node->next == NULL){
                current_node->next = pcbToAdd;
                return;
            }
        }
    }
}

// Finds the node of a linked list at a given index, and returns the node (without modifying it)
struct customQueueNode* getNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToGetAt)(
    customQueueNode* current_node = headCustomQueueNode;
    int currentIndex = 0;
    for (; current_node != NULL; current_node = current_node->next){
        if (currentIndex == indexToGetAt){
            return current_node;
        }

        currentIndex++;
    }

    // if we never returned a node, a node at that index doesn't exist
    return NULL;
)

// Removes the node of a linked list at a given index, and returns the index of the node 
int removeNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToRemoveAt){
    customQueueNode* current_node = headCustomQueueNode;
    customQueueNode* previous_node = NULL;
    int currentIndex = 0;

    for (; current_node != NULL; current_node = current_node->next){
        if (currentIndex == indexToRemoveAt){
            // if we're removing the first node, and its next node is null
            if (currentIndex == 0 && current_node->next == NULL){
                // just set the head of the linked list to NULL
                headCustomQueueNode = NULL;
                return 0; // return 0 (signifies node at index 0 was deleted)
            }
            
            // if we're not removing the first node, we want to have the previous one link to
            previous_node->next = current_node->next;
            int indexOfNodeDeleted = current_node->index;
            free(current_node);
            current_node = NULL;
            return indexOfNodeDeleted; // return indexOfNodeDeleted (signifies node at that index was deleted)
        }

        // if we aren't at the node yet, set the previous node to our current node before advancing it
        previous_node = current_node;
        currentIndex++;
    }

    // if we never returned the function, a node at that index doesn't exist
    return -1;
}

void memorySetup() {
    partitionArray[0].number = 1; partitionArray[0].size = 40; partitionArray[0].occupyingPID = -1;
    partitionArray[1].number = 2; partitionArray[1].size = 25; partitionArray[1].occupyingPID = -1;
    partitionArray[2].number = 3; partitionArray[2].size = 15; partitionArray[2].occupyingPID = -1;
    partitionArray[3].number = 4; partitionArray[3].size = 10; partitionArray[3].occupyingPID = -1;
    partitionArray[4].number = 5; partitionArray[4].size = 8; partitionArray[4].occupyingPID = -1;
    partitionArray[5].number = 6; partitionArray[5].size = 2; partitionArray[5].occupyingPID = -1;

    for (int i = 0; i < 100; i++){
        PCBArray[i].PID = 0;
    }
}

void allocateMemory(struct PCB* pcb) {

}

void terminateProgram(struct PCB* pcb) {
   partitionArray[pcb->partitionInUse - 1].occupyingPID = -1;
   pcb->partitionInUse = 0;
   pcb->state = TERMINATED;
}

bool programs_done() { //checks if program is done
    for (int i = 0; PCBArray[i].PID == 0; i++) {
        if (PCBArray[i].state != TERMINATED) {
            return false;
        }
    }
    return true;
}

bool programRunning(){
    for (int i = 0; PCBArray[i].PID == 0; i++) {
        if (PCBArray[i].state == RUNNING) {
            return true;
        }
    }

    return false;
}

struct PCB* selectNextReadyProgram(struct customQueueNode* headReadyQueueNode) {
    // initialize the earliest 
    int readyQueueLength = customQueueLength(headReadyQueueNode);

    if (readyQueueLength == 0){
        return NULL;
    }
    else if (readyQueueLength == 1){
        struct PCB* pcbToReturn = headReadyQueueNode->pcb; // get the pcb from the node
        free(headReadyQueueNode); // free the head ready queue node
        headReadyQueueNode = NULL; // set the head of the ready queue node list to NULL
        return pcbToReturn; // return the pcb we took before freeing the node
    }
    else{
        // readyQueueLength >= 2
        // need to choose the best to select

        struct customQueueNode* earliestArrivingNode = headReadyQueueNode;
        customQueueNode* current_node = headReadyQueueNode->next;

        // find the earliest arriving node
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node arrived earlier than
            if (current_node->queueTimeArrival < earliestArrivingNode){
                earliestArrivingNode = current_node;
            }
        }

        current_node = headReadyQueueNode;
        // find the node with the highest pid of those that have arrived the earliest (in the case where there are multiple earliest arriving nodes)
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node arrived at the same time as the earliest arriving node
            if (current_node->queueTimeArrival == earliestArrivingNode->queueTimeArrival){
                // if th ecurrent node has a lower pid
                if (current_node->pcb->PID < earliestArrivingNode->pcb->PID){
                    earliestArrivingNode = current_node;
                }
            }
        }

        // by this time we've selected the earliest arriving node, and if there are multiple, we chose the one with the lowest pid
        struct PCB* pcbSelected = earliestArrivingNode->pcb;
        int indexOfNodeToDelete = earliestArrivingNode->index;
        removeNodeAtIndex(headReadyQueueNode, indexOfNodeToDelete); // remove the node that we selected from the ready queue
        return pcbSelected; // return the pcb of the program that we've selected to run
    }

    // the function should never get to this point, if so, return NULL
    return NULL;
}

void FcfsScheduler() {
    struct PCB* runningPCB = NULL;
    unsigned int runTimeLeft = 0;
    struct customQueueNode* headReadyQueueNode = NULL;
    struct customQueueNode* headWaitingQueueNode = NULL;

    while (!programs_done()) {
        
        // checking what's arrived (NEW)
        for (int i = 0; PCBArray[i].PID == 0; i++) { //this loop checks each PCB in PCB array for if any process has arrived
            // if the current program has arrived and is new
            if (PCBArray[i].Arrival_Time >= cpu_time && PCBArray[i].state == NEW) {
                // look for a partition to assign it to from lowest to highest available memory       
                for (int j = 5; j >= 0; j--){
                    // if the current partition is available
                    if (partitionArray[j].occupyingPID == -1){
                        // if the size available in the current partition is enough to store the program
                        if (partitionArray[j].size >= PCBArray[i].Mem_Size){
                            // set partition and pcb information to match each other (we store this program in this partition)
                            PCBArray[i].partitionInUse = partitionArray[j].number;
                            partitionArray[j].occupyingPID = PCBArray[i].PID;
                            // this program should go from new to ready now
                            PCBArray[i].state = READY;
                            customQueueAddNode(headReadyQueueNode, PCBArray[i], cpu_time);
                            break; // break because the search for an available partition is over
                        }
                    }
                }    
            }
        }

        // check if something is running (RUNNING)
        if (runningPCB != NULL){
            // a program is currently running
            if (runTimeLeft > 0){
                if (runningPCB->Remaining_CPU == 0){
                    // terminate the program
                    terminatePCB(runningPCB);
                    runningPCB = NULL;
                }
            } 
            else if (runTimeLeft == 0){
                // runTimeLeft 0, and we haven't terminated the program, so it goes to waiting
                runningPCB->state = WAITING;
                customQueueAddNode(headWaitingQueueNode, runningPCB, cpu_time);
                runningPCB = NULL;
            }
        }
        else{
            // a program needs to be assigned (READY)
            runningPCB = selectNextReadyProgram(headReadyQueueNode);
            runTimeLeft = runningPCB->IO_Freq;
        }

        // process all waiting programs (WAITING)
        struct customQueueNode* current_node = headWaitingQueueNode;
        struct customQueueNode* next_node = NULL;
        for (; current_node != NULL; current_node = next_node){
            int timeSpentWaiting = cpu_time - current_node->queueArrivalTime; // as low as 0
            next_node = current_node->next;

            if (timeSpentWaiting == current_node->IO_Duration){
                // the current program is done waiting
                // assign it back to ready queue
                struct PCB* pcbToAssign = current_node->pcb;
                pcbToAssign->state = READY;
                customQueueAddNode(headReadyQueueNode, pcbToAssign, cpu_time);
                // remove it from waiting queue
                int indexOfNodeToDelete = current_node->index;
                removeNodeAtIndex(headWaitingQueueNode, indexOfNodetoDelete);
            }
        }

        // after checking all PCBs at this time, update time
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
        token = strtok(buffer, ","); // "1"
        PCBArray[counter].Mem_Size = atoi(token);
        token = strtok(buffer, ","); // "0"
        PCBArray[counter].Arrival_Time = atoi(token);
        token = strtok(buffer, ","); // "50"
        PCBArray[counter].CPU_Time = atoi(token);
        token = strtok(buffer, ","); // "10"
        PCBArray[counter].IO_Freq = atoi(token);
        token = strtok(buffer, ","); // "1"
        PCBArray[counter].IO_Duration = atoi(token);

        counter++;
    }
}

void recordStateTransition(int transiton, struct PCB* pcb, int oldState, int newState) {

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