#include <stdio.h>
#include "set.h"
#include <memory.h>


int main(int argc, char* argv[]) {
    //handling input args
    char* input_file = "input4.bin";//file name which want to open
    FILE* fp;
    int ret;
    int fin = 0;
    int cycle = 0;
    sp = 0x1000000;
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

        memory[index] = inst;
        index++;

    }

    fclose(fp);


    //initialize architectural states
    Reg[31] = -1;//initial pc
    Reg[29] = 0x1000000;//initial sp points to the end of memory-> 
    pc = 0;
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
        if (clock % 10000 == 0) {
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
    printf("*********************************************\n");
    //end of exeution
    return 0;
}