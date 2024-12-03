#include "interrupts_101239695_101289957.h"
#define BUFFER_SIZE 32
#define PARTITION_ARRAY_LENGTH 6;
#define MEMORY_IN_PARTITIONS 100

// instantiating arrays
struct partition partitionArray[6];
struct PCB *PCBArray[100];

// =-=     
bool modeBit = 0;
int totalSystemRunTime = 0;
int cpu_time = 0;
// initializing vector table
unsigned int vectorTable[] = {0X01E3, 0X029C, 0X0695, 0X042B, 0X0292, 0X048B, 0X0639, 0X00BD, 0X06EF, 0X036C, 0X07B0, 0X01F8, 0X03B9, 0X06C7, 0X0165, 0X0584, 0X02DF, 0X05B3, 0X060A, 0X0765, 0X07B7, 0X0523, 0X03B7, 0X028C, 0X05E8, 0X05D3};

FILE* traceFilePointer; // create a pointer to the trace file
FILE* outputFilePointer; // create a pointer to the output file
FILE* outputSecondFilePointer; // create a pointer to the memory_status file
char buffer[BUFFER_SIZE]; // create a buffer that is capable of reading each line from the trace file

// customQueue operations
int customQueueLength(struct customQueueNode* headCustomQueueNode){
    int lengthCounter = 0;
    struct customQueueNode* current_node = headCustomQueueNode;

    for (; current_node != NULL; current_node = current_node->next){
        lengthCounter++;
    }

    return lengthCounter++;
}

// Add a node to the end of the ready queue with its relevant information
cqnShorthand *customQueueAddNode(struct customQueueNode* headCustomQueueNode, struct PCB* pcbToAdd, int timeOfArrival){
    struct customQueueNode* nodeToAdd = malloc(sizeof(struct customQueueNode));
    int queueLength = customQueueLength(headCustomQueueNode);
    nodeToAdd->index = queueLength;
    nodeToAdd->pcb = pcbToAdd;
    nodeToAdd->queueArrivalTime = timeOfArrival;
    nodeToAdd->next = NULL;

    if (queueLength == 0){
        headCustomQueueNode = nodeToAdd;
        
        // printf(headCustomQueueNode);
        return headCustomQueueNode;
    }
    else{
        struct customQueueNode* current_node = headCustomQueueNode;
        // Find the last node and set its next node to the nodeToAdd;
        for (; current_node != NULL; current_node = current_node->next){
            if (current_node->next == NULL){
                current_node->next = nodeToAdd;
                // printf(headCustomQueueNode);
                return headCustomQueueNode;
            }
        }
    }
}

// Finds the node of a linked list at a given index, and returns the node (without modifying it)
cqnShorthand *getNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToGetAt){
    struct customQueueNode* current_node = headCustomQueueNode;
    int currentIndex = 0;
    for (; current_node != NULL; current_node = current_node->next){
        if (currentIndex == indexToGetAt){
            return current_node;
        }

        currentIndex++;
    }

    // if we never returned a node, a node at that index doesn't exist
    return NULL;
}

// Removes the node of a linked list at a given index, and returns the index of the node 
cqnShorthand *removeNodeAtIndex(struct customQueueNode* headCustomQueueNode, int indexToRemoveAt){
    struct customQueueNode* current_node = headCustomQueueNode;
    struct customQueueNode* previous_node = NULL;
    int currentIndex = 0;

    for (; current_node != NULL; current_node = current_node->next){
        if (currentIndex == indexToRemoveAt){
            // if we're removing the first node, and its next node is null
            if (currentIndex == 0){
                // 
                headCustomQueueNode = current_node->next;

                // correct the indices of the queue before returning
                int counter = 0;
                for (struct customQueueNode* n = headCustomQueueNode; n != NULL; n = n->next){
                    n->index = counter;
                    counter++;
                }

                return headCustomQueueNode; // return 0 (signifies node at index 0 was deleted)
            }

            previous_node->next = current_node->next;
            free(current_node);
            current_node = NULL;

            
            // correct the indices of the queue before returning
            int counter = 0;
            for (struct customQueueNode* n = headCustomQueueNode; n != NULL; n = n->next){
                n->index = counter;
                counter++;
            }

            return headCustomQueueNode; // return indexOfNodeDeleted (signifies node at that index was deleted)
        }

        // if we aren't at the node yet, set the previous node to our current node before advancing it
        previous_node = current_node;
        currentIndex++;
    }

    // if we never returned the function, a node at that index doesn't exist
    return headCustomQueueNode;
}

void memorySetup() {
    partitionArray[0].number = 1; partitionArray[0].size = 40; partitionArray[0].occupyingPID = -1;
    partitionArray[1].number = 2; partitionArray[1].size = 25; partitionArray[1].occupyingPID = -1;
    partitionArray[2].number = 3; partitionArray[2].size = 15; partitionArray[2].occupyingPID = -1;
    partitionArray[3].number = 4; partitionArray[3].size = 10; partitionArray[3].occupyingPID = -1;
    partitionArray[4].number = 5; partitionArray[4].size = 8; partitionArray[4].occupyingPID = -1;
    partitionArray[5].number = 6; partitionArray[5].size = 2; partitionArray[5].occupyingPID = -1;

    for (int i = 0; i < 100; i++){
        struct PCB* newPCB = malloc(sizeof(struct PCB));
        PCBArray[i] = newPCB;
        PCBArray[i]->PID = 0;
        PCBArray[i]->state = NEW;
    }

    fprintf(outputFilePointer, "+------------------------------------------------+\n");
    fprintf(outputFilePointer, "Time of Transition |PID | Old State | New State |\n");
    fprintf(outputFilePointer, "+------------------------------------------------+\n");

    fprintf(outputSecondFilePointer, "+------------------------------------------------------------------------------------------+\n");
    fprintf(outputSecondFilePointer, "|Time of Event |Memory Used | Partitions State |Total Free Memory |Usable Free Memory \n");
    fprintf(outputSecondFilePointer, "+------------------------------------------------------------------------------------------+\n");
}

struct PCB *terminateProgram(struct PCB* pcb) {
   partitionArray[pcb->partitionInUse - 1].occupyingPID = -1;
   pcb->partitionInUse = 0;
   pcb->state = TERMINATED;

   recordMemoryStatus(outputSecondFilePointer, cpu_time);

    return pcb;
}

bool programs_done() { //checks if program is done
    for (int i = 0; PCBArray[i]->PID != 0; i++) {

        if (PCBArray[i]->state != TERMINATED) {
            return false;
        }
    }
    return true;
}

struct returnPCB *fcfsSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode) {
    int readyQueueLength = customQueueLength(headReadyQueueNode);

    if (readyQueueLength == 0){
        return NULL;
    }
    else if (readyQueueLength == 1){
        struct PCB* pcbToReturn = headReadyQueueNode->pcb; // get the pcb from the node

        struct customQueueNode *nodeToRemove = headReadyQueueNode;
        headReadyQueueNode = NULL;

        free(nodeToRemove);

        struct returnPCB* pcbReturnStruct = malloc(sizeof(struct returnPCB));
        pcbReturnStruct->pcb = pcbToReturn;
        pcbReturnStruct->cqn = headReadyQueueNode;

        return pcbReturnStruct; // return the pcb we took before freeing the node
    }
    else{
        // readyQueueLength >= 2
        // need to choose the best to select
        // printf("two program or more\n");

        struct customQueueNode* earliestArrivingNode = headReadyQueueNode;
        struct customQueueNode* current_node = headReadyQueueNode->next;

        // find the earliest arriving node
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node arrived earlier than
            if (current_node->pcb->Arrival_Time < earliestArrivingNode->pcb->Arrival_Time){
                earliestArrivingNode = current_node;
            }
        }

        current_node = headReadyQueueNode;
        // find the node with the lowest pid of those that have arrived the earliest (in the case where there are multiple earliest arriving nodes)
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node arrived at the same time as the earliest arriving node
            if (current_node->pcb->Arrival_Time == earliestArrivingNode->pcb->Arrival_Time){
                // if th current node has a lower pid
                if (current_node->pcb->PID < earliestArrivingNode->pcb->PID){
                    earliestArrivingNode = current_node;
                }
            }
        }

        // by this time we've selected the earliest arriving node, and if there are multiple, we chose the one with the lowest pid
        struct PCB* pcbSelected = earliestArrivingNode->pcb;
        int indexOfNodeToDelete = earliestArrivingNode->index;
        
        /*
        printf("before removing the node we're trying to select, ready queue looks like: \n");
        for (struct customQueueNode* cNR = headReadyQueueNode; cNR != NULL; cNR = cNR->next){
            printf("Program %d\n", cNR->pcb->PID);
        }
        */

        headReadyQueueNode = removeNodeAtIndex(headReadyQueueNode, indexOfNodeToDelete); // remove the node that we selected from the ready queue

        /*
        printf("after removing the node we're trying to select, ready queue looks like: \n");
        for (struct customQueueNode* cNR2 = headReadyQueueNode; cNR2 != NULL; cNR2 = cNR2->next){
            printf("Program %d\n", cNR2->pcb->PID);
        }
        */

        /*
        printf("Programs in the ready queue after selection should be:\n");
        for (struct customQueueNode* cNR = headReadyQueueNode; cNR != NULL; cNR = cNR->next){
            printf("Program %d\n", cNR->pcb->PID);
        }
        */

        struct returnPCB* pcbReturnStruct = malloc(sizeof(struct returnPCB));
        pcbReturnStruct->pcb = pcbSelected;
        pcbReturnStruct->cqn = headReadyQueueNode;

        return pcbReturnStruct; // return the pcb of the program that we've selected to run
    }

    // the function should never get to this point
}

struct returnPCB *epSelectNextReadyProgram(struct customQueueNode* headReadyQueueNode) {
    int readyQueueLength = customQueueLength(headReadyQueueNode);

    if (readyQueueLength == 0){
        return NULL;
    }
    else if (readyQueueLength == 1){
        struct PCB* pcbToReturn = headReadyQueueNode->pcb; // get the pcb from the node

        struct customQueueNode *nodeToRemove = headReadyQueueNode;
        headReadyQueueNode = NULL;

        free(nodeToRemove);

        struct returnPCB* pcbReturnStruct = malloc(sizeof(struct returnPCB));
        pcbReturnStruct->pcb = pcbToReturn;
        pcbReturnStruct->cqn = headReadyQueueNode;

        return pcbReturnStruct; // return the pcb we took before freeing the node
    }
    else{
        // readyQueueLength >= 2
        // need to choose the best to select
        // printf("two program or more\n");

        struct customQueueNode* earliestQueuedNode = headReadyQueueNode;
        struct customQueueNode* current_node = headReadyQueueNode->next;

        // find the node that has been in the ready queue the longest
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node has been in the ready queue the longest
            if (current_node->queueArrivalTime < earliestQueuedNode->queueArrivalTime){
                earliestQueuedNode = current_node;
            }
        }

        current_node = headReadyQueueNode;
        // find the node with the lowest pid of those that have been in the ready queue the longest (in the case where there are multiple programs that have been in the ready queue the longest)
        for (; current_node != NULL; current_node = current_node->next){
            // if the current node has been in the ready queue for the same amount of time as the earliest queued node
            if (current_node->queueArrivalTime == earliestQueuedNode->queueArrivalTime){ 
                // if the current node has a lower pid
                if (current_node->pcb->PID < earliestQueuedNode->pcb->PID){
                    earliestQueuedNode = current_node;
                }
            }
        }

        // by this time we've selected the node that has been in the ready queue the longest, and if there are multiple, we chose the one with the lowest pid
        struct PCB* pcbSelected = earliestQueuedNode->pcb;
        int indexOfNodeToDelete = earliestQueuedNode->index;

        headReadyQueueNode = removeNodeAtIndex(headReadyQueueNode, indexOfNodeToDelete); // remove the node that we selected from the ready queue

        struct returnPCB* pcbReturnStruct = malloc(sizeof(struct returnPCB));
        pcbReturnStruct->pcb = pcbSelected;
        pcbReturnStruct->cqn = headReadyQueueNode;

        return pcbReturnStruct; // return the pcb of the program that we've selected to run
    }

    // the function should never get to this point
}

void fcfsScheduler() {
    struct PCB* runningPCB = NULL;
    int runTimeLeft = 0;
    struct customQueueNode* headReadyQueueNode = NULL;
    struct customQueueNode* headWaitingQueueNode = NULL;
    cpu_time = 0;
    
    recordMemoryStatus(outputSecondFilePointer, cpu_time);

    while (!programs_done()) {
        // checking what's arrived (NEW)
        // printf("t-%d\n", cpu_time);

        // check if anything stops running at this second
        if (runningPCB != NULL){
            if (runningPCB->Remaining_CPU <= 0 && runTimeLeft != 0){
                // terminate
                // Record execution
                // printf("running -> terminated @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 4); // RUNNING -> TERMINATED
                // terminate the program (records memory status)
                runningPCB = terminateProgram(runningPCB);

                runningPCB = NULL;
            }
            else if (runTimeLeft == 0){
                // runTimeLeft 0, and we haven't terminated the program, so it goes to waiting
                runningPCB->state = WAITING;

                headWaitingQueueNode = customQueueAddNode(headWaitingQueueNode, runningPCB, cpu_time);

                // Record execution output
                // printf("running -> waiting @ %d with pid %d\n\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 3); // RUNNING -> WAITING

                // reset runningPCB to NULL
                runningPCB = NULL;
            }
        }

        for (int i = 0; PCBArray[i]->PID != 0; i++) { //this loop checks each PCB in PCB array for if any process has arrived
            // printf("Check what's arrived?\n");
            // if the current program has arrived and is new
            if (PCBArray[i]->Arrival_Time == cpu_time && PCBArray[i]->state == NEW) {
                // look for a partition to assign it to from lowest to highest available memory       
                for (int j = 5; j >= 0; j--){
                    // if the current partition is available
                    if (partitionArray[j].occupyingPID == -1){
                        // if the size available in the current partition is enough to store the program
                        if (partitionArray[j].size >= PCBArray[i]->Mem_Size){
                            // printf("found a match1\n");
                            // set partition and pcb information to match each other (we store this program in this partition)
                            PCBArray[i]->partitionInUse = partitionArray[j].number;
                            partitionArray[j].occupyingPID = PCBArray[i]->PID;
                            // this program should go from new to ready now
                            PCBArray[i]->state = READY; // set to READY (1)
                            headReadyQueueNode = customQueueAddNode(headReadyQueueNode, PCBArray[i], cpu_time);

                            // Record execution + memory_status output
                            // printf("NEW -> READY @ %d\n", cpu_time);
                            recordStateTransition(outputFilePointer, cpu_time, PCBArray[i]->PID, 0, 1); // NEW -> READY
                            recordMemoryStatus(outputSecondFilePointer, cpu_time);
                            break; // break because the search for an available partition is over
                        }
                    }
                }    
            }
        }

        // process all waiting programs (WAITING)
        struct customQueueNode* current_node = headWaitingQueueNode;
        struct customQueueNode* next_node = NULL;

        for (; current_node != NULL; current_node = next_node){
            int timeSpentWaiting = cpu_time - current_node->queueArrivalTime; // as low as 0
            next_node = current_node->next; // point to next node independently, since we might delete the current node and still need to jump to the next

            if (timeSpentWaiting == current_node->pcb->IO_Duration){
                // the current program is done waiting
                // gotta remove from the waiting queue
                // +
                // assign it back to ready queue
                struct PCB* pcbToAssign = current_node->pcb;

                pcbToAssign->state = READY;
                
                headReadyQueueNode = customQueueAddNode(headReadyQueueNode, pcbToAssign, cpu_time);
                // Record execution output
                // printf("waiting -> ready @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, pcbToAssign->PID, 3, 1); // WAITING -> READY

                // remove it from waiting queue
                int indexOfNodeToDelete = current_node->index;

                headWaitingQueueNode = removeNodeAtIndex(headWaitingQueueNode, indexOfNodeToDelete);

            }
        }

        if (runningPCB == NULL){
            // a program needs to be assigned (READY)

            struct returnPCB* returnInformation = fcfsSelectNextReadyProgram(headReadyQueueNode);

            if (returnInformation != NULL){
                headReadyQueueNode = returnInformation->cqn;
                runningPCB = returnInformation->pcb;
                
                free(returnInformation);
                returnInformation = NULL;
            }

            // printf(runningPCB);
            if (runningPCB != NULL){ // if we found a valid program to run
                runTimeLeft = runningPCB->IO_Freq;

                // Record execution output
                // printf("ready -> running @ %d with pid %d\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 1, 2); // READY -> RUNNING
            }
        }

        cpu_time++;

        // check if something is running (RUNNING)
        if (runningPCB != NULL){
            // a program is currently running
            (runningPCB->Remaining_CPU)--;
            runTimeLeft--;
        }
        
    }
}

void PriorityScheduler() {
    struct PCB* runningPCB = NULL;
    int runTimeLeft = 0;
    struct customQueueNode* headReadyQueueNode = NULL;
    struct customQueueNode* headWaitingQueueNode = NULL;
    cpu_time = 0;
    
    recordMemoryStatus(outputSecondFilePointer, cpu_time);

    while (!programs_done()) {
        // checking what's arrived (NEW)
        // printf("t-%d\n", cpu_time);

        // check if anything stops running at this second
        if (runningPCB != NULL){
            if (runningPCB->Remaining_CPU <= 0 && runTimeLeft != 0){
                // terminate
                // Record execution
                // printf("running -> terminated @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 4); // RUNNING -> TERMINATED
                // terminate the program (records memory status)
                runningPCB = terminateProgram(runningPCB);

                runningPCB = NULL;
            }
            else if (runTimeLeft == 0){
                // runTimeLeft 0, and we haven't terminated the program, so it goes to waiting
                runningPCB->state = WAITING;

                headWaitingQueueNode = customQueueAddNode(headWaitingQueueNode, runningPCB, cpu_time);

                // Record execution output
                // printf("running -> waiting @ %d with pid %d\n\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 3); // RUNNING -> WAITING

                // reset runningPCB to NULL
                runningPCB = NULL;
            }
        }

        for (int i = 0; PCBArray[i]->PID != 0; i++) { //this loop checks each PCB in PCB array for if any process has arrived
            // printf("Check what's arrived?\n");
            // if the current program has arrived and is new
            if (PCBArray[i]->Arrival_Time == cpu_time && PCBArray[i]->state == NEW) {
                // look for a partition to assign it to from lowest to highest available memory       
                for (int j = 5; j >= 0; j--){
                    // if the current partition is available
                    if (partitionArray[j].occupyingPID == -1){
                        // if the size available in the current partition is enough to store the program
                        if (partitionArray[j].size >= PCBArray[i]->Mem_Size){
                            // printf("found a match1\n");
                            // set partition and pcb information to match each other (we store this program in this partition)
                            PCBArray[i]->partitionInUse = partitionArray[j].number;
                            partitionArray[j].occupyingPID = PCBArray[i]->PID;
                            // this program should go from new to ready now
                            PCBArray[i]->state = READY; // set to READY (1)
                            headReadyQueueNode = customQueueAddNode(headReadyQueueNode, PCBArray[i], cpu_time);

                            // Record execution + memory_status output
                            // printf("NEW -> READY @ %d\n", cpu_time);
                            recordStateTransition(outputFilePointer, cpu_time, PCBArray[i]->PID, 0, 1); // NEW -> READY
                            recordMemoryStatus(outputSecondFilePointer, cpu_time);
                            break; // break because the search for an available partition is over
                        }
                    }
                }    
            }
        }

        // process all waiting programs (WAITING)
        struct customQueueNode* current_node = headWaitingQueueNode;
        struct customQueueNode* next_node = NULL;

        for (; current_node != NULL; current_node = next_node){
            int timeSpentWaiting = cpu_time - current_node->queueArrivalTime; // as low as 0
            next_node = current_node->next; // point to next node independently, since we might delete the current node and still need to jump to the next

            if (timeSpentWaiting == current_node->pcb->IO_Duration){
                // the current program is done waiting
                // gotta remove from the waiting queue
                // +
                // assign it back to ready queue
                struct PCB* pcbToAssign = current_node->pcb;

                pcbToAssign->state = READY;
                
                headReadyQueueNode = customQueueAddNode(headReadyQueueNode, pcbToAssign, cpu_time);
                // Record execution output
                // printf("waiting -> ready @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, pcbToAssign->PID, 3, 1); // WAITING -> READY

                // remove it from waiting queue
                int indexOfNodeToDelete = current_node->index;

                headWaitingQueueNode = removeNodeAtIndex(headWaitingQueueNode, indexOfNodeToDelete);

            }
        }

        if (runningPCB == NULL){
            // a program needs to be assigned (READY)

            struct returnPCB* returnInformation = epSelectNextReadyProgram(headReadyQueueNode);

            if (returnInformation != NULL){
                headReadyQueueNode = returnInformation->cqn;
                runningPCB = returnInformation->pcb;
                
                free(returnInformation);
                returnInformation = NULL;
            }

            // printf(runningPCB);
            if (runningPCB != NULL){ // if we found a valid program to run
                runTimeLeft = runningPCB->IO_Freq;

                // Record execution output
                // printf("ready -> running @ %d with pid %d\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 1, 2); // READY -> RUNNING
            }
        }

        cpu_time++;

        // check if something is running (RUNNING)
        if (runningPCB != NULL){
            // a program is currently running
            (runningPCB->Remaining_CPU)--;
            runTimeLeft--;
        }
        
    }
}

void RoundRobinScheduler() {
    struct PCB* runningPCB = NULL;
    int runTimeLeft = 0;
    int runTimeElapsed = 0;
    int const timeoutValue = 100;
    struct customQueueNode* headReadyQueueNode = NULL;
    struct customQueueNode* headWaitingQueueNode = NULL;
    cpu_time = 0;
    
    recordMemoryStatus(outputSecondFilePointer, cpu_time);

    while (!programs_done()) {
        // checking what's arrived (NEW)
        // printf("t-%d\n", cpu_time);

        // check if anything stops running at this second
        if (runningPCB != NULL){
            if (runningPCB->Remaining_CPU <= 0 && runTimeLeft != 0){
                // terminate
                // Record execution
                // printf("running -> terminated @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 4); // RUNNING -> TERMINATED
                // terminate the program (records memory status)
                runningPCB = terminateProgram(runningPCB);

                runningPCB = NULL;
            }
            else if (runTimeLeft == 0 || runTimeElapsed == timeoutValue){
                // runTimeLeft 0, and we haven't terminated the program, so it goes to waiting
                runningPCB->state = WAITING;

                headWaitingQueueNode = customQueueAddNode(headWaitingQueueNode, runningPCB, cpu_time);

                // Record execution output
                // printf("running -> waiting @ %d with pid %d\n\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 2, 3); // RUNNING -> WAITING

                // reset runningPCB to NULL
                runningPCB = NULL;
            }
        }

        for (int i = 0; PCBArray[i]->PID != 0; i++) { //this loop checks each PCB in PCB array for if any process has arrived
            // printf("Check what's arrived?\n");
            // if the current program has arrived and is new
            if (PCBArray[i]->Arrival_Time == cpu_time && PCBArray[i]->state == NEW) {
                // look for a partition to assign it to from lowest to highest available memory       
                for (int j = 5; j >= 0; j--){
                    // if the current partition is available
                    if (partitionArray[j].occupyingPID == -1){
                        // if the size available in the current partition is enough to store the program
                        if (partitionArray[j].size >= PCBArray[i]->Mem_Size){
                            // printf("found a match1\n");
                            // set partition and pcb information to match each other (we store this program in this partition)
                            PCBArray[i]->partitionInUse = partitionArray[j].number;
                            partitionArray[j].occupyingPID = PCBArray[i]->PID;
                            // this program should go from new to ready now
                            PCBArray[i]->state = READY; // set to READY (1)
                            headReadyQueueNode = customQueueAddNode(headReadyQueueNode, PCBArray[i], cpu_time);

                            // Record execution + memory_status output
                            // printf("NEW -> READY @ %d\n", cpu_time);
                            recordStateTransition(outputFilePointer, cpu_time, PCBArray[i]->PID, 0, 1); // NEW -> READY
                            recordMemoryStatus(outputSecondFilePointer, cpu_time);
                            break; // break because the search for an available partition is over
                        }
                    }
                }    
            }
        }

        // process all waiting programs (WAITING)
        struct customQueueNode* current_node = headWaitingQueueNode;
        struct customQueueNode* next_node = NULL;

        for (; current_node != NULL; current_node = next_node){
            int timeSpentWaiting = cpu_time - current_node->queueArrivalTime; // as low as 0
            next_node = current_node->next; // point to next node independently, since we might delete the current node and still need to jump to the next

            if (timeSpentWaiting == current_node->pcb->IO_Duration){
                // the current program is done waiting
                // gotta remove from the waiting queue
                // +
                // assign it back to ready queue
                struct PCB* pcbToAssign = current_node->pcb;

                pcbToAssign->state = READY;
                
                headReadyQueueNode = customQueueAddNode(headReadyQueueNode, pcbToAssign, cpu_time);
                // Record execution output
                // printf("waiting -> ready @ %d\n", cpu_time);
                recordStateTransition(outputFilePointer, cpu_time, pcbToAssign->PID, 3, 1); // WAITING -> READY

                // remove it from waiting queue
                int indexOfNodeToDelete = current_node->index;

                headWaitingQueueNode = removeNodeAtIndex(headWaitingQueueNode, indexOfNodeToDelete);

            }
        }

        if (runningPCB == NULL){
            // a program needs to be assigned (READY)

            struct returnPCB* returnInformation = fcfsSelectNextReadyProgram(headReadyQueueNode);

            if (returnInformation != NULL){
                headReadyQueueNode = returnInformation->cqn;
                runningPCB = returnInformation->pcb;
                
                free(returnInformation);
                returnInformation = NULL;
            }

            // printf(runningPCB);
            if (runningPCB != NULL){ // if we found a valid program to run
                runTimeLeft = runningPCB->IO_Freq;
                runTimeElapsed = 0;

                // Record execution output
                // printf("ready -> running @ %d with pid %d\n", cpu_time, runningPCB->PID);
                recordStateTransition(outputFilePointer, cpu_time, runningPCB->PID, 1, 2); // READY -> RUNNING
            }
        }

        cpu_time++;

        // check if something is running (RUNNING)
        if (runningPCB != NULL){
            // a program is currently running
            (runningPCB->Remaining_CPU)--;
            runTimeLeft--;
            runTimeElapsed++;
        }
        
    }
}

void InputFileProcesser(FILE* traceFilePointer) {
    // 15, 10, 0, 25, 11, 3 instruction parsing
    unsigned int counter = 0;

    while(fgets(buffer, BUFFER_SIZE, traceFilePointer)) {
        char *token = strtok(buffer, ","); // "15"
        PCBArray[counter]->PID = atoi(token);
        token = strtok(NULL, ","); // "10"
        PCBArray[counter]->Mem_Size = atoi(token);
        token = strtok(NULL, ","); // "0"
        PCBArray[counter]->Arrival_Time = atoi(token);
        token = strtok(NULL, ","); // "25"
        PCBArray[counter]->CPU_Time = atoi(token);
        token = strtok(NULL, ","); // "11"
        PCBArray[counter]->IO_Freq = atoi(token);
        token = strtok(NULL, ","); // "3"
        PCBArray[counter]->IO_Duration = atoi(token);

        PCBArray[counter]->Remaining_CPU = PCBArray[counter]->CPU_Time;

        counter++;
    }
}

void recordStateTransition(FILE* execution_file, int timeOfTransition, unsigned int pid, int oldState, int newState) {
    // execution_file line print

    // string var for old and new state name to print
    char oldStateName[20];
    char newStateName[20];

    // Translate the state number to its state name 
    // reason we do it this way is because its actually impossible to access the name of our enum state at run time
    switch (oldState){
        case 0: strcpy(oldStateName, "NEW"); break;
        case 1: strcpy(oldStateName, "READY"); break;
        case 2: strcpy(oldStateName, "RUNNING"); break;
        case 3: strcpy(oldStateName, "WAITING"); break;
        case 4: strcpy(oldStateName, "TERMINATED"); break;
    }
    switch (newState){
        case 0: strcpy(newStateName, "NEW"); break;
        case 1: strcpy(newStateName, "READY"); break;
        case 2: strcpy(newStateName, "RUNNING"); break;
        case 3: strcpy(newStateName, "WAITING"); break;
        case 4: strcpy(newStateName, "TERMINATED"); break;
    }

    fprintf(execution_file, "| %d               | %d | %s       | %s        | \n", timeOfTransition, pid, oldStateName, newStateName);
}

void recordMemoryStatus(FILE* memory_status_file, int timeOfEvent) {
    // memory_status_file line print

    // calculate total free memory
    int totalFreeMemory = 0;
    int usableFreeMemory = 0;

    for (int i = 0; i < 6; i++){
        if (partitionArray[i].occupyingPID == -1){
            totalFreeMemory += partitionArray[i].size;
            usableFreeMemory += partitionArray[i].size;
        }
        else{
            // partition is occupied by a program
            for (int j = 0; PCBArray[j]->PID != 0; j++) {
                if (PCBArray[j]->PID == partitionArray[i].occupyingPID){
                    // this is the program that occupies it
                    totalFreeMemory += (partitionArray[i].size - PCBArray[j]->Mem_Size);
                    break; // found the program that occupies it, go to next partition
                }
            }
        }
    }

    // calculate memory used
    int memoryUsed = 100 - totalFreeMemory;

    fprintf(memory_status_file, "| %d           | %d         |", timeOfEvent, memoryUsed);
    
    for (int i = 0; i < 6; i++){
        if (i == 5){
            fprintf(memory_status_file, "%d |", partitionArray[i].occupyingPID);
        }
        else{
            fprintf(memory_status_file, "%d, ", partitionArray[i].occupyingPID);
        }
    }

    fprintf(memory_status_file, " %d            | %d            |\n", totalFreeMemory, usableFreeMemory);
}

int main(int argc, char* argv[])
{
    // assign MyTraceFile1.txt to traceFilePointer in read mode
    traceFilePointer = fopen("input_test.txt", "r"); // argv[0]

    // take the chosen algorithm from the second argument
    char chosenAlgorithm[10];
    strcpy(chosenAlgorithm, "RR"); // argv[1]

    // assign ProgramOutput1.txt to outputFilePointer in write mode
    outputFilePointer = fopen("execution.txt", "w");

    // assign system_status.txt to outputSecondFilePointer in write mode
    outputSecondFilePointer = fopen("memory_status.txt", "w");

    // initializing partition array and pcb array
    memorySetup();

    // load PCBs from input file
    InputFileProcesser(traceFilePointer);

    if (strcmp(chosenAlgorithm, "FCFS") == 0){
        fcfsScheduler();
    }
    else if (strcmp(chosenAlgorithm, "EP") == 0){
        PriorityScheduler();
    }
    else if (strcmp(chosenAlgorithm, "RR") == 0){
        RoundRobinScheduler();
    }
    else{
        printf("Invalid submission! ERROR!!!!\n");
    }

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