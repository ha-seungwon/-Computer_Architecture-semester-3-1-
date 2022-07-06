#include <stdio.h>
#include "set.h"
#include <memory.h>


void AlwaysNotTakenPredictCheck(int b_address, int b_taken, int bpc) {
    if (!b_taken) {//not taken
        predict_correction += 1;
    }
    else {// taken
        Flush1();
        Flush2();
        pc = b_address;
        mis_predict += 1;
    }
}


void AlwaysTakenPredictCheck(int b_address, int b_taken, int bpc) {
    if (BTB[bpc] != 0) {//Predict to taken
        if (b_taken) {//taken
            predict_correction += 1;
        }
        else {//not taken
            Flush1();
            Flush2();
            pc = bpc + 4;
            mis_predict += 1;
        }
    }
    else {
        if (!b_taken) {//not taken
            predict_correction += 1;
        }
        else {// taken
            Flush1();
            Flush2();
            BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;
        }
    }
}
void BTFNPredictCheck(int b_address, int b_taken, int bpc) {
    if (BTB[bpc] != 0) {
        if (b_taken) {
            predict_correction += 1;
        }
        else {
            Flush1();
            Flush2();
            pc = bpc + 4;
            mis_predict += 1;
        }
    }
    else {
        if (!b_taken) {//not taken
            predict_correction += 1;
        }
        else {// taken
            Flush1();
            Flush2();
            if (b_address < bpc)BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;
        }
    }
}

void OneBitBranchPredictCheck(int b_address, int b_taken, int bpc) {
    if (BTB[bpc] != 0) {
        if (B_Taken[bpc] == 0x1) {//predict to taken
            if (b_taken) {//taken
                B_Taken[bpc] += 1;
                if (B_Taken[bpc] > 0x1) {
                    B_Taken[bpc] = 0x1;
                }
                predict_correction += 1;
            }
            else {//not taken
                Flush1();
                Flush2();
                B_Taken[bpc] -= 1;
                if (B_Taken[bpc] < 0x0) {
                    B_Taken[bpc] = 0x0;
                }
                pc = bpc + 4;
                mis_predict += 1;
            }
        }
        else {//predict to not taken
            if (!b_taken) {//not taken
                B_Taken[bpc] -= 1;
                if (B_Taken[bpc] < 0x0) {
                    B_Taken[bpc] = 0x0;
                }
                predict_correction += 1;
            }
            else {// taken
                Flush1();
                Flush2();
                B_Taken[bpc] += 1;
                if (B_Taken[bpc] > 0x1) {
                    B_Taken[bpc] = 0x1;
                }
                pc = b_address;
                mis_predict += 1;
            }
        }
    }
    else {
        if (!b_taken) {//not taken
            //printf("predict\n");
            B_Taken[bpc] -= 1;
            if (B_Taken[bpc] < 0x0) {
                B_Taken[bpc] = 0x0;
            }
            predict_correction += 1;
        }
        else {// taken
            Flush1();
            Flush2();
            //printf("mis predict\n");
            B_Taken[bpc] += 1;
            if (B_Taken[bpc] > 0x1) {
                B_Taken[bpc] = 0x1;
            }
            BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;

        }
    }
}

void TwoBitBranchPredictCheck(int b_address, int b_taken, int bpc) {
    if (BTB[bpc] != 0) {
        if (B_Taken[bpc] >= 2) {//predict to taken
            if (b_taken) {//taken
                B_Taken[bpc] += 1;
                if (B_Taken[bpc] > 3) {
                    B_Taken[bpc] = 3;
                }
                predict_correction += 1;
            }
            else {//not taken
                Flush1();
                Flush2();
                B_Taken[bpc] -= 1;
                if (B_Taken[bpc] < 0x00) {
                    B_Taken[bpc] = 0x00;
                }
                pc = bpc + 4;
                mis_predict += 1;
            }
        }
        else {//predict to not taken
            if (!b_taken) {//not taken
                B_Taken[bpc] -= 1;
                if (B_Taken[bpc] < 0x00) {
                    B_Taken[bpc] = 0x00;
                }
                predict_correction += 1;
            }
            else {// taken
                Flush1();
                Flush2();
                B_Taken[bpc] += 1;
                if (B_Taken[bpc] > 3) {
                    B_Taken[bpc] = 3;
                }
                pc = b_address;
                mis_predict += 1;
            }
        }
    }
    else {
        if (b_taken) {
            Flush1();
            Flush2();
            B_Taken[bpc] += 1;
            if (B_Taken[bpc] > 3) {
                B_Taken[bpc] = 3;
            }
            BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;
        }
    }
}

void TwoLevelPredictCheck(int b_address, int b_taken, int bpc) {

    if (BTB[bpc] != 0) {
        if (B_Taken[bpc ^ GHR] >= 2) {//predict to taken
            if (b_taken) {//taken

                B_Taken[bpc ^ GHR] += 1;
                if (B_Taken[bpc ^ GHR] > 3) {
                    B_Taken[bpc ^ GHR] = 3;
                }
                predict_correction += 1;
            }
            else {//not taken
                Flush1();
                Flush2();
                B_Taken[bpc ^ GHR] -= 1;
                if (B_Taken[bpc ^ GHR] < 0x00) {
                    B_Taken[bpc ^ GHR] = 0x00;
                }
                pc = bpc + 4;
                mis_predict += 1;
            }
        }
        else {//predict to not taken
            if (!b_taken) {//not taken
                B_Taken[bpc ^ GHR] -= 1;
                if (B_Taken[bpc ^ GHR] < 0x00) {
                    B_Taken[bpc ^ GHR] = 0x00;
                }
                predict_correction += 1;
            }
            else {// taken
                Flush1();
                Flush2();
                B_Taken[bpc ^ GHR] += 1;
                if (B_Taken[bpc ^ GHR] > 3) {
                    B_Taken[bpc ^ GHR] = 3;
                }
                pc = b_address;
                mis_predict += 1;
            }
        }
    }
    else {
        if (b_taken) {
            Flush1();
            Flush2();
            B_Taken[bpc ^ GHR] += 1;
            if (B_Taken[bpc ^ GHR] > 3) {
                B_Taken[bpc ^ GHR] = 3;
            }
            BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;
        }
    }
    GHR << 1;
    GHR = GHR | b_taken;
}

void LocalTwoLevelPredictCheck(int b_address, int b_taken, int bpc) {
    if (BTB[bpc] != 0) {
        if (B_Taken[BPHT[bpc]] >= 2) {//predict to taken
            if (b_taken) {//taken
                B_Taken[BPHT[bpc]] += 1;
                if (B_Taken[BPHT[bpc]] > 3) {
                    B_Taken[BPHT[bpc]] = 3;
                }
                predict_correction += 1;
            }
            else {//not taken
                Flush1();
                Flush2();
                B_Taken[BPHT[bpc]] -= 1;
                if (B_Taken[BPHT[bpc]] < 0x00) {
                    B_Taken[BPHT[bpc]] = 0x00;
                }
                pc = bpc + 4;
                mis_predict += 1;
            }
        }
        else {//predict to not taken
            if (!b_taken) {//not taken
                B_Taken[BPHT[bpc]] -= 1;
                if (B_Taken[BPHT[bpc]] < 0x00) {
                    B_Taken[BPHT[bpc]] = 0x00;
                }
                predict_correction += 1;
            }
            else {// taken
                Flush1();
                Flush2();
                B_Taken[BPHT[bpc]] += 1;
                if (B_Taken[BPHT[bpc]] > 3) {
                    B_Taken[BPHT[bpc]] = 3;
                }
                pc = b_address;
                mis_predict += 1;
            }
        }
    }
    else {
        if (b_taken) {
            Flush1();
            Flush2();
            B_Taken[BPHT[bpc]] += 1;
            if (B_Taken[BPHT[bpc]] > 3) {
                B_Taken[BPHT[bpc]] = 3;
            }
            BTB[bpc] = b_address;
            pc = b_address;
            mis_predict += 1;
        }
    }
    BPHT[bpc] << 1;
    BPHT[bpc] |= b_taken;
}


void Flush1() {
    latch1[0].valid = 0;
    latch1[1].valid = 0;
}
void Flush2() {
    latch2[0].valid = 0;
    latch2[1].valid = 0;
}