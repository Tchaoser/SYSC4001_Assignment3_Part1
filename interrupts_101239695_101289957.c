#include "interrupts_101239695_101289957.h"
#define BUFFER_SIZE 32

struct partition {
    unsigned int number;
    unsigned int size;
    char code[20];
};

struct program {
    char name[20];
    unsigned int sizeWhenLoaded;
};

struct PCB {
    unsigned int PID;
    struct program theProgram;
    unsigned int partitionInUse;
};

// instantiating arrays
struct partition partitionArray[6];
struct program programArray[100];
struct PCB PCBArray[100];

// =-=     
bool modeBit = 0;
int totalSystemRunTime = 0;
char initName[20];
// initializing vector table
unsigned int vectorTable[] = {0X01E3, 0X029C, 0X0695, 0X042B, 0X0292, 0X048B, 0X0639, 0X00BD, 0X06EF, 0X036C, 0X07B0, 0X01F8, 0X03B9, 0X06C7, 0X0165, 0X0584, 0X02DF, 0X05B3, 0X060A, 0X0765, 0X07B7, 0X0523, 0X03B7, 0X028C, 0X05E8, 0X05D3};

// instruction evaluator(s)
const char cpuInstruction[] = "CPU, ";
const char forkInstruction[] = "FORK, ";
const char execInstruction[] = "EXEC ";

FILE* currentFilePointer;
FILE* externalFilePointer; // create a pointer to the external files file
FILE* outputSecondFilePointer; // create a pointer to the system_status file
FILE* traceFilePointer; // create a pointer to the trace file
FILE* outputFilePointer; // create a pointer to the output file
char buffer[BUFFER_SIZE]; // create a buffer that is capable of reading each line from the trace file

void runCPU(int CPURunTime, FILE* outputToFilePointer){
    
    // CPU operations
    fprintf(outputToFilePointer, "%d, %d, CPU execution\n", totalSystemRunTime, CPURunTime);
    totalSystemRunTime += CPURunTime;
}

void runSYSCALL(int vector, unsigned int pcAddress, int ISRBodyRunTime, FILE* outputToFilePointer){

    // OVERHEAD operations
    modeBit = 1;
    fprintf(outputToFilePointer, "%d, 1, switch to kernel mode\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    srand(time(0));
    int randomContextSwitchTime = (rand() % 3) + 1;
    fprintf(outputToFilePointer, "%d, %d, context saved\n", totalSystemRunTime, randomContextSwitchTime);
    totalSystemRunTime += randomContextSwitchTime;
    fprintf(outputToFilePointer, "%d, 1, find vector %d in memory position %d\n", totalSystemRunTime, vector, vector * 2);
    totalSystemRunTime += 1;
    fprintf(outputToFilePointer, "%d, 1, load address 0x%x into the PC\n", totalSystemRunTime, pcAddress);
    totalSystemRunTime += 1;

    // SYSCALL operations
    //  ISR body - Divide into 3 random blocks of time that sum to given ISR body time
    int firstDivision = ISRBodyRunTime/3;
    srand(time(0));
    int rando = (rand() % firstDivision) + 1;
    int Time1 = firstDivision + rando; // create 1st time block
    int secondDivision = (ISRBodyRunTime - Time1) / 2;
    rando = (rand() % secondDivision) + 1;
    int Time2 = secondDivision + rando; // create 2nd time block
    int Time3 = ISRBodyRunTime - Time1 - Time2; // create 3rd time block
    fprintf(outputToFilePointer, "%d, %d, SYSCALL: run the ISR\n", totalSystemRunTime, Time1);
    totalSystemRunTime += Time1;
    fprintf(outputToFilePointer, "%d, %d, transfer data\n", totalSystemRunTime, Time2);
    totalSystemRunTime += Time2;
    fprintf(outputToFilePointer, "%d, %d, check for errors\n", totalSystemRunTime, Time3);
    // check that the vector address is valid *
    totalSystemRunTime += Time3;

    // OVERHEAD operations
    // IRET
    fprintf(outputToFilePointer, "%d, 1, IRET\n", totalSystemRunTime);
    totalSystemRunTime += 1;

    modeBit = 0; // no time impact in simulation
}

void runENDIO(int vector, unsigned int pcAddress, int ENDIOBodyRunTime, FILE* outputToFilePointer){

    // OVERHEAD operations
    // check priority of interrupt
    fprintf(outputToFilePointer, "%d, 1, check priority of interrupt\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    // check if masked 
    fprintf(outputToFilePointer, "%d, 1, check if masked\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    // switch to kernel mode
    modeBit = 1;
    fprintf(outputToFilePointer, "%d, 1, switch to kernel mode\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    // context saved
    srand(time(0));
    int randomContextSwitchTime = (rand() % 3) + 1;
    fprintf(outputToFilePointer, "%d, %d, context saved\n", totalSystemRunTime, randomContextSwitchTime);
    totalSystemRunTime += randomContextSwitchTime;
    // find vector x in memory position y
    fprintf(outputToFilePointer, "%d, 1, find vector %d in memory position %d\n", totalSystemRunTime, vector, vector * 2);
    totalSystemRunTime += 1;
    // load address into PC
    fprintf(outputToFilePointer, "%d, 1, load address 0x%x into the PC\n", totalSystemRunTime, pcAddress);
    totalSystemRunTime += 1;
    
    // END_IO operations 
    // END_IO body
    fprintf(outputToFilePointer, "%d, %d, END_IO\n", totalSystemRunTime, ENDIOBodyRunTime);
    totalSystemRunTime += ENDIOBodyRunTime;

    // OVERHEAD operations
    // IRET
    fprintf(outputToFilePointer, "%d, 1, IRET\n", totalSystemRunTime);
    totalSystemRunTime += 1;

    modeBit = 0; // no time impact in simulation
}

void routineScheduler(){
    // scheduler called
}

void runFORK(int vector, unsigned int pcAddress, int FORKBodyRunTime, FILE* outputToFilePointer, FILE* outputToSecondFilePointer){
    // OVERHEAD
    fprintf(outputToFilePointer, "%d, 1, switch to kernel mode\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    srand(time(0));
    int randomContextSwitchTime = (rand() % 3) + 1;
    fprintf(outputToFilePointer, "%d, %d, context saved\n", totalSystemRunTime, randomContextSwitchTime);
    totalSystemRunTime += randomContextSwitchTime;
    fprintf(outputToFilePointer, "%d, 1, find vector %d in memory position %d\n", totalSystemRunTime, vector, vector * 2);
    totalSystemRunTime += 1;
    fprintf(outputToFilePointer, "%d, 1, load address 0x%x into the PC\n", totalSystemRunTime, pcAddress);
    totalSystemRunTime += 1;

    // randomly divide FORKBodyRunTime between the 2 tasks in FORK body
    int firstDivision = FORKBodyRunTime/2;
    srand(time(0));
    int rando = (rand() % firstDivision) + 1;
    int Time1 = firstDivision + rando; // create 1st time block
    int Time2 = FORKBodyRunTime - Time1; // create 2nd time block

    fprintf(outputToFilePointer, "%d, %d, FORK: copy parent PCB to child PCB\n", totalSystemRunTime, Time1);
    totalSystemRunTime += Time1;
    fprintf(outputToFilePointer, "%d, %d, scheduler called\n", totalSystemRunTime, Time2);
    totalSystemRunTime += Time2;

    // find the first unset index in PCBArray 
    int pcbIndex;
    for (pcbIndex = 0; pcbIndex < 100; pcbIndex++){
        // if the memory size to be loaded is 0, we know the PCB has not been used yet
        if (PCBArray[pcbIndex].theProgram.sizeWhenLoaded == 0){
            break;
        }
    }

    // copy parent PCB to child PCB
    PCBArray[pcbIndex].PID = pcbIndex;
    strcpy(PCBArray[pcbIndex].theProgram.name, PCBArray[0].theProgram.name);

    PCBArray[pcbIndex].theProgram.sizeWhenLoaded = PCBArray[0].theProgram.sizeWhenLoaded;
    PCBArray[pcbIndex].partitionInUse = PCBArray[0].partitionInUse;

    // print status
    //printSystemStatus();

    // OVERHEAD
    // IRET
    fprintf(outputToFilePointer, "%d, 1, IRET\n", totalSystemRunTime);
    totalSystemRunTime += 1;
}

void runEXEC(int vector, unsigned int pcAddress, char *theProgramName, int EXECBodyRunTime, FILE* outputToFilePointer, FILE* outputToSecondFilePointer){
    

    // OVERHEAD
    fprintf(outputToFilePointer, "%d, 1, switch to kernel mode\n", totalSystemRunTime);
    totalSystemRunTime += 1;
    srand(time(0));
    int randomContextSwitchTime = (rand() % 3) + 1;
    fprintf(outputToFilePointer, "%d, %d, context saved\n", totalSystemRunTime, randomContextSwitchTime);
    totalSystemRunTime += randomContextSwitchTime;
    fprintf(outputToFilePointer, "%d, 1, find vector %d in memory position %d\n", totalSystemRunTime, vector, vector * 2);
    totalSystemRunTime += 1;
    fprintf(outputToFilePointer, "%d, 1, load address 0x%x into the PC\n", totalSystemRunTime, pcAddress);
    totalSystemRunTime += 1;

    // actual work
    // cycle through program table until we find the desired program
    // we'll want to load last set PCB with
    // program name and size => direct
    // partition number => systematically compare program size with partitions to find best-fit

    int programIndex;
    for (programIndex = 0; programIndex < 100; programIndex++){
        if (strcmp(programArray[programIndex].name, theProgramName) == 0){
            // found our program at programIndex
            break;
        }
    }

    int pcbIndex;
    for (pcbIndex = 0; pcbIndex < 100; pcbIndex++){
        if (PCBArray[pcbIndex].theProgram.sizeWhenLoaded == 0){
            // found the first unset PCB, and we want the PCB just before that
            pcbIndex = pcbIndex - 1;
            break;
        }
    }

    // potential error
    strcpy(PCBArray[pcbIndex].theProgram.name, programArray[programIndex].name);
    PCBArray[pcbIndex].theProgram.sizeWhenLoaded = programArray[programIndex].sizeWhenLoaded;

    // finding best-fit in available partitions
    unsigned int partitionIndex = 0;
    int bestSizeDifference = partitionArray[0].size - PCBArray[pcbIndex].theProgram.sizeWhenLoaded;

    for (int k = 0; k < 6; k++){
        if (strcmp(partitionArray[k].code, "free") == 0){
            // partition is available for consideration
            int tempDifference = partitionArray[k].size - PCBArray[pcbIndex].theProgram.sizeWhenLoaded;
            if (tempDifference >= 0 && tempDifference < bestSizeDifference){
                bestSizeDifference = tempDifference;
                partitionIndex = k;
            }
        }
    }

    PCBArray[pcbIndex].partitionInUse = partitionArray[partitionIndex].number;

    // match partition to new data (change code from free to the current program name)
    strcpy(partitionArray[partitionIndex].code, PCBArray[pcbIndex].theProgram.name);

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

    fprintf(outputFilePointer, "%d, %d, EXEC: load %s of size %uMb\n", totalSystemRunTime, Time1, theProgramName, PCBArray[pcbIndex].theProgram.sizeWhenLoaded);
    totalSystemRunTime += Time1;
    fprintf(outputFilePointer, "%d, %d, found partition %u with %uMb of space\n", totalSystemRunTime, Time2, PCBArray[pcbIndex].partitionInUse, PCBArray[pcbIndex].theProgram.sizeWhenLoaded);
    totalSystemRunTime += Time2;
    fprintf(outputFilePointer, "%d, %d, partition %u marked as occupied\n", totalSystemRunTime, Time3, PCBArray[pcbIndex].partitionInUse);
    totalSystemRunTime += Time3;
    fprintf(outputFilePointer, "%d, %d, updating PCB with new information\n", totalSystemRunTime, Time4);
    totalSystemRunTime += Time4;
    fprintf(outputFilePointer, "%d, %d, scheduler called\n", totalSystemRunTime, Time5);
    totalSystemRunTime += Time5;

    // print status
    //printSystemStatus();

    // OVERHEAD
    // IRET
    fprintf(outputToFilePointer, "%d, 1, IRET\n", totalSystemRunTime);
    totalSystemRunTime += 1;

}

void runProgram(FILE* currentProgram){
    while(fgets(buffer, BUFFER_SIZE, currentProgram)) {
        // Process command-line instruction
        int vectorNum;
        int callRunTime;
        char programName[20];

        if (strstr(buffer, cpuInstruction) != NULL){ // if the buffer contains a CPU instruction
            // CPU, CPURunTime instruction parsing
            char *token = strtok(buffer, " "); // "CPU,"
            token = strtok(NULL, " "); // "systemRunTime"
            callRunTime = atoi(token);
            
            vectorNum = -1;
        }
        else if (strstr(buffer, forkInstruction) != NULL){ // if the buffer contains a FORK instruction
            // FORK, FORKBodyRunTime instruction parsing
            char *token = strtok(buffer, " "); // "FORK,"
            token = strtok(NULL, " "); // "FORKBodyRunTime"
            callRunTime = atoi(token);

            vectorNum = 2;
        }
        else if (strstr(buffer, execInstruction) != NULL){ // if the buffer contains an EXEC instruction
            // INSTRUCTION_CODE programName, programBodyRunTime instruction parsing
            char *token = strtok(buffer, " "); // "INSTRUCTION_CODE"
            token = strtok(NULL, " "); // "programName,"
            token[strlen(token) - 1] = '\0'; // "programName"
            char name[20];
            strcpy(name, token);
            strcpy(programName, strcat(name, ".txt")); // copy "programName.txt" to programName
            token = strtok(NULL, " "); // "programBodyRunTime"
            callRunTime = atoi(token);

            vectorNum = 3;
        }
        else{
            // INSTRUCTION_CODE vector, callBodyRunTime instruction parsing
            char *token = strtok(buffer, " "); // "INSTRUCTION_CODE"
            token = strtok(NULL, " "); // "vector,"
            token[strlen(token) - 1] = '\0';
            vectorNum = atoi(token);
            token = strtok(NULL, " "); // "callBodyRunTime"
            callRunTime = atoi(token);
        }

        if (vectorNum < 0){
            // CPU instruction
            runCPU(callRunTime, outputFilePointer);
        }
        else{
            unsigned int pcAddress = vectorTable[vectorNum]; // accessing pcAddress from vector table (hardcoded)
            
            if (pcAddress == 0X01E3) {
                // do nothing (divide by 0 ISR)
            }
            else if (pcAddress == 0X029C) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0695) {
                // FORK
                runFORK(vectorNum, pcAddress, callRunTime, outputFilePointer, outputSecondFilePointer);
            }
            else if (pcAddress == 0X042B) {
                // EXEC
                unsigned int sizeOfProgram;
                for (int i = 0; i < 100; i++){
                    // if programArray[i].name == programName, we'll get the size of programName
                    if (strcmp(programArray[i].name, programName) == 0){
                        sizeOfProgram = programArray[i].sizeWhenLoaded;
                        break;
                    }
                }

                runEXEC(vectorNum, pcAddress, programName, callRunTime, outputFilePointer, outputSecondFilePointer);

                // point to current EXEC. program
                FILE *tempProgramStore = currentProgram;

                currentProgram = fopen(programName, "r");
                runProgram(currentProgram);

                currentProgram = tempProgramStore;
            }
            else if (pcAddress == 0X0292) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X048B) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0639) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X00BD) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X06EF) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X036C) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X07B0) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X01F8) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X03B9) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X06C7) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0165) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0584) {
                runSYSCALL(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X02DF) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X05B3) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X060A) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0765) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X07B7) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X0523) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X03B7) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X028C) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X05E8) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            }
            else if (pcAddress == 0X05D3) {
                runENDIO(vectorNum, pcAddress, callRunTime, outputFilePointer);
            } 
        }
        
        // Move onto next command-line instruction
    }
}

int main(int argc, char* argv[])
{
    strcpy(initName, argv[1]); // argv[1]

    // assign external_files.txt to externalFilePointer in read mode
    externalFilePointer = fopen("external_files_101239695_101289957.txt", "r");

    // initializing program array
    // for loop, basically a while "there is still another line" loop with an i counter 
    // which increases with each line to represent proper program array index

    int c = 0;

    while (fgets(buffer, BUFFER_SIZE, externalFilePointer)){

        // programName, programSize instruction parsing
        struct program theProgram; // create a new program theProgram
        char *token = strtok(buffer, " "); // "programName,"
        token[strlen(token) - 1] = '\0'; // "programName"
        char name[20];
        strcpy(name, token);
        strcpy(theProgram.name, strcat(name, ".txt")); // saving "programName.txt" in theProgram.name
        token = strtok(NULL, " "); // "programSize"
        theProgram.sizeWhenLoaded = atoi(token); // saving "programSize" in theProgram.sizeWhenLoaded
        
        // store this program in programArray
        programArray[c] = theProgram;

        c++;
    }

    // initializing pcb array
    PCBArray[0].PID = 0;
    strcpy(PCBArray[0].theProgram.name, argv[1]); // argv[1]

    PCBArray[0].theProgram.sizeWhenLoaded = 1;
    PCBArray[0].partitionInUse = 6;

    for (int i = 1; i < 100; i++){
        PCBArray[i].theProgram.sizeWhenLoaded = 0;
    }

    // assign MyTraceFile1.txt to traceFilePointer in read mode
    traceFilePointer = fopen(argv[1], "r"); // argv[1]

    // assign ProgramOutput1.txt to outputFilePointer in write mode
    outputFilePointer = fopen(argv[2], "w"); // argv[2]

    // assign system_status.txt to outputSecondFilePointer in write mode
    outputSecondFilePointer = fopen(argv[3], "w"); // argv[3]

    // initializing partition array
    partitionArray[0].number = 1; partitionArray[0].size = 40; strcpy(partitionArray[0].code, "free");
    partitionArray[1].number = 2; partitionArray[1].size = 25; strcpy(partitionArray[1].code, "free");
    partitionArray[2].number = 3; partitionArray[2].size = 15; strcpy(partitionArray[2].code, "free");
    partitionArray[3].number = 4; partitionArray[3].size = 10; strcpy(partitionArray[3].code, "free");
    partitionArray[4].number = 5; partitionArray[4].size = 8; strcpy(partitionArray[4].code, "free");
    partitionArray[5].number = 6; partitionArray[5].size = 2; strcpy(partitionArray[5].code, argv[1]); // argv[1]

    // system_status initial print
    //printSystemStatus();

    runProgram(traceFilePointer);

    // End of instructions, close program

    fclose(traceFilePointer);
    fclose(outputFilePointer);
    return 0;   
}