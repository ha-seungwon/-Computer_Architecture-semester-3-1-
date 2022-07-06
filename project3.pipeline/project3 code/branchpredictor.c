#include <stdio.h>
#include "set.h"
#include <memory.h>



int LocalTwoLevelPredict(int pc) {
    int hit = 0;
    int taken = 0;
    if (BTB[pc] != 0) {
        hit = 1;
        switch (B_Taken[BPHT[pc]])
        {
        case 0:
        case 1:
            taken = 0;
            break;
        case 2:
        case 3:
            taken = 1;
            break;
        }

    }
    return hit && taken;
}

int TwoLevelPredict(int GHR, int pc) {
    int hit = 0;
    int taken = 0;
    if (BTB[pc] != 0) {
        hit = 1;
        switch (B_Taken[pc ^ GHR])
        {
        case 0:
        case 1:
            taken = 0;
            break;
        case 2:
        case 3:
            taken = 1;
            break;
        }

    }
    return hit && taken;
}

int TwoBitBranchPredict(int pc) {
    int hit = 0;
    int taken = 0;
    if (BTB[pc] != 0) {
        hit = 1;
        switch (B_Taken[pc])
        {
        case 0:
        case 1:
            taken = 0;
            break;
        case 2:
        case 3:
            taken = 1;
            break;
        }
    }
    return hit && taken;
}

int OneBitBranchPredict(int pc) {
    int hit = 0;
    int taken = 0;
    if (BTB[pc] != 0) {
        hit = 1;
        switch ((int)B_Taken[pc])
        {
        case 0x0:
            taken = 0;
            break;
        case 0x1:
            taken = 1;
            break;
        }
    }
    return hit && taken;

}

int BTFN(int pc) {
    if (BTB[pc] != 0) {
        if (BTB[pc] < pc)return 1;
        return 0;
    }
    else {
        return 0;
    }
}


int AlwaysTakenPredict(int pc) {
    if (BTB[pc] != 0) {
        return 1;
    }
    else {
        return 0;
    }

}
