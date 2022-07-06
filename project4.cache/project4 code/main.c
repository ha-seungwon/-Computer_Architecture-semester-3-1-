#include <stdio.h>
#include "set.h"
#include <memory.h>
#include <math.h>

int main(int argc, char* argv[]) {
    //handling input args
    char* input_file = "input4.bin";//file name which want to open
    FILE* fp;
    int ret;
    int fin = 0;
    int cycle = 0;
    sp = 0x1000000000;

    memset(&memory, 0, sizeof(memory));
    memset(&instmemory, 0, sizeof(instmemory));
    //load the program
    if (argc == 2) {
        input_file = argv[1];
    }
    //open the file
    fp = fopen(input_file, "rb");
    int index = 0;
    //read from the file
    while (1) {
        unsigned char in[4];
        int inst;
        int* ptr;
        for (int i = 0; i < 4; i++) {
            ret = fread(&in[i], 1, 1, fp);
            if (ret != 1) {
                fin = 1;
            }
        }

        // change the position of the instruction by 1byte-> explain
        unsigned tmp;
        tmp = in[1];
        in[1] = in[2];
        in[2] = tmp;
        tmp = in[3];
        in[3] = in[0];
        in[0] = tmp;
        ptr = &in[0];
        inst = *ptr;
        if (fin == 1) break;

        instmemory[index] = inst;
        //memory[index] = inst;
        index++;

    }

    fclose(fp);


    //initialize architectural states
    Reg[31] = -1;//initial pc
    Reg[29] = 0x100000;//initial sp points to the end of memory-> 
    pc = 0;
    front=0;   // 삭제 위치
    rear=0;
    cachereplace = 0;
    coldMiss = 0;

    for (int i = 0; i < 2; i++) {
        latch1[i].valid = 0;
        latch2[i].valid = 0;
        latch3[i].valid = 0;
        latch4[i].valid = 0;
    }

    memset(&latch1, 0, sizeof(latch1));
    memset(&latch2, 0, sizeof(latch2));
    memset(&latch3, 0, sizeof(latch3));
    memset(&latch4, 0, sizeof(latch4));
    for (int i = 0; i < 2; i++) {
        memset(&cache2way[i], 0, sizeof(cache2way));
        for (int j = 0; j < 64; j++) {
            memset(&cache2queue[j].queue, 0, sizeof(cache2queue[j].queue));
            memset(&cache2lru[j], 0, sizeof(cache2lru[j]));
        }
    }

    for (int i = 0; i < 4; i++) {
        memset(&cache4way[i], 0, sizeof(cache4way));
        for (int j = 0; j < 32; j++) {
            memset(&cache4queue[j].queue, 0, sizeof(cache4queue[j].queue));
            memset(&cache4lru[j], 0, sizeof(cache4lru[j]));
        }
    }

    for (int i = 0; i < 8; i++) {
        memset(&cache8way[i], 0, sizeof(cache4way));
        for (int j = 0; j < 16; j++) {
            memset(&cache8queue[j].queue, 0, sizeof(cache8queue[j].queue));
            memset(&cache8lru[j], 0, sizeof(cache8lru[j]));
        }
    }
    
    for (int i = 0; i < 128; i++) {
        memset(&FAcache[i], 0, sizeof(FAcache));
        memset(&FAcachequeue.queue, 0, sizeof(FAcachequeue));
        

    }
    memset(&FAcachelru, 0, sizeof(FAcachelru));
    memset(&directmapcache, 0, sizeof(directmapcache));
    

    //begin execution loop
    while (1) {
        //in the loop..
        memaccess();
        wb();

        if (pc == -1) {
            break;
        }
        fetch();
        decode();
        execute();

        Switch();
        if (clock % 100000 == 0) {
           printf("clcokcycle %d\n ", clock);

        }
        clock++;
    }


    //result
    printf("*********************************************\n");
    printf("Cycle: %d\n", clock);
    printf("Result -> R[2]: %d\n", Reg[2]);//print out result (result will be store in the register number 2)
    printf("Number of instructions: %d\n", numOfInsts - 2 * mis_predict);
    printf("Number of memory access instructions: %d\n", memoryops);
    printf("Number of Register ops: %d\n", regops - mis_predict);
    printf("Number of branch instruction: %d\n", branchinsts);
    printf("Number of jump instruction: %d\n", jumpinsts);
    printf("Predict correct : %d  , mis predict : %d ,total predict: %d\n", predict_correction, mis_predict, (predict_correction + mis_predict));
    if ((mis_predict + predict_correction) != 0)printf("Accurate : %d\n", 100 * predict_correction / (mis_predict + predict_correction));
    printf("mismem cache access: %d, hitmem cache access: %d\n", cachemiss, cachehit);
    if ((cachemiss + cachehit) != 0) { 
        printf("cache Accurate : %d\n", (100 * cachehit / (cachemiss + cachehit)));
        printf("cache confilct miss: %d\n", cachereplace);
        printf("cold miss: %d\n", coldMiss);
        printf("AMAT: %d ns\n", (cachehit * 10 + cachemiss * 1000));
    }
    
    printf("*********************************************\n");
    //end of exeution
    return 0;
}