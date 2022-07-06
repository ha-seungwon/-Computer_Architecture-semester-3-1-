#include <stdio.h>
#include "set.h"
#include <memory.h>


void flush_f(int regdata) {
    pc = regdata;
    latch1[1].valid = 0;
    numOfInsts--;
    //printf("flush_f!!\n");
}

void fetch() {
    //printf("________________________\n"); 
    //printf("fetch\n");
    //printf("pc: 0x%x\n", pc);
    inst = memory[pc / 4];
    //printf("inst: 0x%x\n", inst);
    latch1[0].inst = inst;
    numOfInsts++;
    latch1[0].pc_4 = pc + 4;
    //AlwaysNotTaken
    //pc = pc + 4;

    /*
    //Alwaystaken
    
    if (AlwaysTakenPredict(pc)) {
        pc = BTB[pc];
    }
    else {
        pc = pc + 4;
    }*/
    
    /*
    //BTFN
    if (BTFN(pc)) {
        pc = BTB[pc];
    }
    else {
        pc = pc + 4;
    }
   */
    /*
    //OneBitPredict
    if (OneBitBranchPredict(pc)) {
        pc = BTB[pc];
    }
    else {
        pc = pc + 4;
    }*/
      
    
    /*
    //TwoBitPredict

    if(TwoBitBranchPredict(pc)){
        pc = BTB[pc];
    }
    else {
        pc = pc + 4;
    }*/

    /*
    
    //TwoLevelPredict
    if (TwoLevelPredict(GHR, pc)) {
        pc=BTB[pc];
    }
    else {
        pc = pc + 4;
    }*/
    
    
    //LocalTwoLevelPredict
    if (LocalTwoLevelPredict(pc)) {
        pc = BTB[pc];
    }
    else {
        pc = pc + 4;
    }
    
    
    //printf("pc updata : 0x%x\n", pc);
 
    //printf("inst: 0x%x\n", latch1[0].inst);
    latch1[0].valid = 1;


}


int control(int opcode, int funct) {//function to initialize the control signal
    latch2[0].ex.RegDst = (opcode == 0);//r type instrution 
    if (opcode == 0x2b)latch2[0].ex.RegDst = 1;
    latch2[0].ex.ALUSrc = (opcode != 0) && (opcode != 0x4) && (opcode != 0x5);
    latch2[0].ex.jr = (opcode == 0x0) && (funct == 0x08);//jr mux signal.
    latch2[0].ex.branch = (opcode == 0x4) || (opcode == 0x5);//branch.
    latch2[0].ex.ALUOp = opcode;
    latch2[0].ex.jal = (opcode == 0x3);

    latch2[0].m.memoryRead = opcode == 0x23;//load word
    latch2[0].m.memoryWrite = opcode == 0x2b;//store word
    latch2[0].m.PCSrc1 = (opcode == 0x2) || (opcode == 0x3); //jump or not



    latch2[0].wb.PCSrc2 = (opcode == 0x4) && (opcode == 0x5);//bcond statisfied
    latch2[0].wb.j_and_l_toReg = (opcode == 0x3);
    latch2[0].wb.memtoReg = opcode == 0x23;
    latch2[0].wb.RegWrite = (opcode != 0x4) && (opcode != 0x5) && (opcode != 0x2) && (opcode != 0x2b);




    if (opcode == 0x3) {// jump and link
        latch2[0].ex.RegDst = 2;
        latch2[0].wb.RegWrite = 1;
    }
    if (opcode == 0xd) {//or immediate
        latch2[0].ex.zero = 1;
    }
}


void decode() {
    if (latch1[1].valid == 0)return;
    //printf("________________________\n");
    //printf("decode\n");
    inst = latch1[1].inst;
    //printf("inst: 0x%x\n", inst);
    int rs = 0;
    int rd = 0;
    int rt = 0;
    unsigned int imm = 0;
    int jtarget = 0;
    unsigned int funct = 0;
    int shamt = 0;


    unsigned int temp_in = inst;
    int opcode = (inst >> 26) & 0x3f;  // get some opcode bit from the instruction -> explain

    rs = (inst >> 21) & 0x1f;
    rt = (inst >> 16) & 0x1f;
    rd = (inst >> 11) & 0x1f;
    if (opcode != 0)rd = -1;
    shamt = (inst >> 6) & 0x1f;
    funct = inst & 0x3f;


    imm = temp_in & 0xffff;
    if (opcode == 0x2 || opcode == 0x3) {//jump and jump and link( j type instruction)
        jtarget = temp_in & 0x3ffffff;
        jumpinsts++;
    }


    // printf("opcode : 0x%x\n", opcode);
     //if (opcode == 0x0)printf("fun: 0x%x\n", funct);

    control(opcode, funct);//initialize the control signals by opcode.->explain
   

    //reg    
    reg(rs, rt);

    if (opcode == 0x2 || opcode == 0x3) {// shift 2bits and add the current upper pc 4bit to use jump address->explain
        int tempj = jtarget << 2;
        pc = (tempj + (latch1[1].pc_4 & (0xf << 28)));
        //printf("jump pc updatex: 0x%x\n", pc);

    }
    // printf("rs,rt,rd: %d , %d, %d\n", rs, rt,rd);

    int temp = imm;

    //sign extend
    if (temp >> 15 == 0) {//imm sign extend
        latch2[0].simm = 0xffffffff & imm;
    }
    else {
        latch2[0].simm = (0xffff << 16) | imm;
    }

    if (latch2[0].ex.zero) {//zeroextend of ori
        int zeroextend = imm;
        latch2[0].simm = 0xffffffff & imm;


    }
    //printf("simm : 0x%x\n", latch2[0].simm);

    latch2[0].rs = rs;
    latch2[0].rd = rd;
    latch2[0].rt = rt;
    latch2[0].shamt = shamt;
    latch2[0].imm = imm;
    latch2[0].valid = 1;
    
    latch2[0].pc_4 = latch1[1].pc_4;
    //printf("pc: 0x%x\n", latch2[0].pc);
    return;


}


int reg(int reg1, int reg2) {
    regops++;
    int data1 = 0;
    int data2 = 0;


    //get data from the register
    data1 = Reg[reg1];
    data2 = Reg[reg2];

    latch2[0].Rdata1 = data1;
    latch2[0].Rdata2 = data2;
    ////printf("data1,data2: 0x%x,0x%x\n", latch2[0].Rdata1, latch2[0].Rdata2);

    return 0;
}

//execution
void execute() {
    if (latch2[1].valid == 0)return;
    //printf("________________________\n");
    //printf("execute\n");

    int data1 = latch2[1].Rdata1;
    int data2 = latch2[1].Rdata2;

    switch ((int)ForwardA(latch2[1].rs, latch3[1].regDst, latch4[1].regDst)) {
    case 0:

        break;

    case 1:
        data1 = latch3[1].aluResult;

        break;
    case 2:

        if (latch4[1].wb.RegWrite == 1) {
            if (latch4[1].wb.memtoReg == 1) {
                data1 = latch4[1].mReadData;

            }
            else if (latch4[1].wb.memtoReg == 0) {
                data1 = latch4[1].aluResult;

            }
            else if (latch4[1].wb.j_and_l_toReg == 1) {
                data1 = latch4[1].pc_4;


            }
        }

        break;
    }
    switch ((int)ForwardB(latch2[1].rt, latch3[1].regDst, latch4[1].regDst)) {
    case 0:

        break;

    case 1:

        data2 = latch3[1].aluResult;

        break;
    case 2:
        if (latch4[1].wb.RegWrite == 1) {
            if (latch4[1].wb.memtoReg == 1) {
                data2 = latch4[1].mReadData;

            }
            else if (latch4[1].wb.memtoReg == 0) {
                data2 = latch4[1].aluResult;

            }
            else if (latch4[1].wb.j_and_l_toReg == 1) {
                data2 = latch4[1].pc_4;


            }
        }
        break;
    }



    if (latch2[1].ex.jr) {
        //printf("flushdata: %d\n", data1);
        flush_f(data1);
        jumpinsts++;

    }

    latch3[0].writeData = data2;

    //i type or not
    if (latch2[1].ex.ALUSrc) {
        ALU(data1, latch2[1].simm);
    }
    else {
        ALU(data1, data2);
    }

    if (latch2[1].ex.RegDst == 0) {
        latch3[0].regDst = latch2[1].rt;
    }
    else if (latch2[1].ex.RegDst == 1) {
        latch3[0].regDst = latch2[1].rd;
    }
    else if (latch2[1].ex.RegDst == 2) {
        latch3[0].regDst = 31;
    }


    latch3[0].pc_4 = latch2[1].pc_4;



    //pointer?
    latch3[0].m.memoryRead = latch2[1].m.memoryRead;
    latch3[0].m.memoryWrite = latch2[1].m.memoryWrite;
    latch3[0].m.PCSrc1 = latch2[1].m.PCSrc1;


    latch3[0].wb.memtoReg = latch2[1].wb.memtoReg;
    latch3[0].wb.PCSrc2 = latch2[1].wb.PCSrc2;
    latch3[0].wb.RegWrite = latch2[1].wb.RegWrite;
    latch3[0].wb.j_and_l_toReg = latch2[1].wb.j_and_l_toReg;


}

int ForwardA(int rs, int  regDst1, int regDst2) {
    if (rs == regDst1)return 1;
    else if (rs == regDst2)return 2;
    else return 0;
}
int ForwardB(int rt, int  regDst1, int regDst2) {
    if (rt == regDst1)return 1;
    else if (rt == regDst2)return 2;
    else return 0;
}


int ALUcontrol(int op) {
    if (op == 0x0) {
        return (latch2[1].simm & 0x3f);
    }
    else {
        return op;
    }

}


int ALU(int data1, int data2) {
    //printf("ALUSrc: %d\n", latch2[1].ex.ALUSrc);


    unsigned int result = 0;
    // printf("data1: 0x%x , data2 0x%x\n", data1, data2);
    // printf("latch2[1].ex.ALUOp 0x%x\n", latch2[1].ex.ALUOp);
   //  printf("ALUcontrol(latch2[1].ex.ALUOp) : %d\n", ALUcontrol(latch2[1].ex.ALUOp));
    switch (latch2[1].ex.ALUOp) {
    case 0x8://addi
            //printf("addi \n");
        result = data1 + data2;
        break;

    case 0x9://addiu
           // printf("addiu \n");

        result = data1 + data2;
        ////printf("%d + %d= %d\n", data1, data2,result);
        break;

    case 0x2://jump here we just initlized the control signals
            //printf("j \n");
        break;

    case 0x3://jump and link
            //printf("jal \n");
        break;
    case 0x2b://sw

        result = data1 + data2;
        //printf("sw result : 0x%x\n",result);
        break;
    case 0x23:
        //printf("lw \n");
        result = data1 + data2;
        break;
    case 0xa://slti
            //printf("slti\n");
            //printf("data1: 0x%x < data2:0x%x =%d\n", data1, data2, (data1 < data2));
        if (data1 < data2)result = 1;
        break;
    case 0xd: //ori     
        result = data1 | data2;
        break;
    case 0xf://lui
        result = latch2[1].imm << 16;
        break;

    case 0x4://beq
        branchinsts++;
        //printf("beq\n");
        //printf("data1: 0x%x = data2:0x%x =%d\n", data1, data2, (data1 == data2));
        if (data1 == data2) {
            becond = 1;
            jumpinsts++;
        }
        else {
            becond = 0;
        }
        break;
    case 0x5://bne
        branchinsts++;
        //printf("bne\n");
        //printf("data1: 0x%x != data2:0x%x =%d\n",data1,data2, (data1 != data2));
        if (data1 != data2) {
            becond = 1;
            jumpinsts++;
        }
        else {
            becond = 0;
        }
        break;
    case 0x0:
        switch (ALUcontrol(latch2[1].ex.ALUOp)) {
        case 0x20://add
            result = data1 + data2;
            break;

        case 0x21://addu
            result = data1 + data2;
            //printf("addu : 0x%x \n",result);
            break;
        case 0x08://jump register
            //printf("jr \n");
            result = 0;
            break;
        case 0x2a:
            if (data1 < data2)result = 1;
            break;
        case 0x22:
            result = data1 - data2;
            break;
        case 0x02://srl
            result = data2 >> latch2[1].shamt;
            break;

        case 0x00://s l logical
            //printf("sll \n");
            result = data2 << latch2[1].shamt;
            break;

        case 0x23://subtract unsigned
            result = data1 - data2;

            break;
        }
    }



    //brach con
    int b_taken = latch2[1].ex.branch & becond;
    int b_address = (latch2[1].pc_4) + (latch2[1].simm << 2);

    if (latch2[1].ex.ALUOp == 0x4 || latch2[1].ex.ALUOp == 0x5) {
        
        //AlwaysNotTakenPredictCheck(b_address, b_taken, latch2[1].pc_4 - 4);
        //AlwaysTakenPredictCheck(b_address, b_taken, latch2[1].pc_4-4);
        //BTFNPredictCheck(b_address, b_taken, latch2[1].pc_4 - 4);
        //OneBitBranchPredictCheck(b_address, b_taken, latch2[1].pc_4-4);
        //TwoBitBranchPredictCheck(b_address, b_taken, latch2[1].pc_4-4);
        //TwoLevelPredictCheck(b_address, b_taken, latch2[1].pc_4 - 4);
        LocalTwoLevelPredictCheck(b_address, b_taken, latch2[1].pc_4 - 4);
    }


    latch3[0].aluResult = result;
    latch3[0].valid = 1;

    //printf("result: 0x%x\n", latch3[0].aluResult);

}

void memaccess() {
    if (latch3[1].valid == 0)return;
    //printf("________________________\n");
    //printf("memaccess\n");


    mem(latch3[1].aluResult, latch3[1].writeData);
    latch4[0].aluResult = latch3[1].aluResult;
    latch4[0].pc_4 = latch3[1].pc_4 + 4;
    latch4[0].regDst = latch3[1].regDst;
    latch4[0].wb.memtoReg = latch3[1].wb.memtoReg;
    latch4[0].wb.PCSrc2 = latch3[1].wb.PCSrc2;
    latch4[0].wb.RegWrite = latch3[1].wb.RegWrite;
    latch4[0].valid = 1;
    latch4[0].wb.j_and_l_toReg = latch3[1].wb.j_and_l_toReg;
}

int mem(int address, int writeData) {//memory
    //////printf("memmory access\n");
    //printf("memory sigal: r=%d w=%d\n",latch3[1].m.memoryRead, latch3[1].m.memoryWrite);
    //printf("rd: %d\n", latch3[1].regDst);
    if (latch3[1].m.memoryWrite == 1) {//sw
        // i need control dependency !!
        //printf("sw address : 0x%x\n",address);
        //printf("sw data : 0x%x\n", writeData);
        memory[address / 4] = writeData;
        memoryops++;
        return 0;
    }
    else if (latch3[1].m.memoryRead == 1) {//lw   
        //printf("lw\n");
        latch4[0].mReadData = memory[address / 4];
        //printf("lw data : 0x%x\n",latch4[0].mReadData);
        memoryops++;
        return 0;
    }

    else if (latch3[1].m.memoryRead == 0 && latch3[1].m.memoryRead == 0) {
        return 0;
    }

}

void wb() {
    if (latch4[1].valid == 0)return;
    //printf("________________________\n");
    //printf("wb\n");
    //printf("regw,memtoreg,jal: %d %d %d\n", latch4[1].wb.RegWrite, latch4[1].wb.memtoReg, latch4[1].wb.j_and_l_toReg);
    if (latch4[1].wb.RegWrite == 1) {

        if (latch4[1].wb.memtoReg == 1) {
            Reg[latch4[1].regDst] = latch4[1].mReadData;
            //printf("Reg[%d]: 0x%x\n", latch4[1].regDst, latch4[1].mReadData);
        }
        else if (latch4[1].wb.memtoReg == 0) {
            if (latch4[1].wb.j_and_l_toReg == 1) {
                //printf("j_and_l_toReg\n");
                Reg[latch4[1].regDst] = latch4[1].pc_4;
                jumpinsts++;
                //printf("Reg[%d]: 0x%x\n", latch4[1].regDst, latch4[1].pc);
            }
            else {
                Reg[latch4[1].regDst] = latch4[1].aluResult;
                //printf("Reg[%d]: 0x%x\n", latch4[1].regDst, latch4[1].aluResult);
            }
        }
    }
    return;
}

void Switch() {
    if (latch1[0].valid == 1) {
        latch1[1] = latch1[0];
    }
    if (latch2[0].valid == 1) {
        latch2[1] = latch2[0];
    }
    if (latch3[0].valid == 1) {
        latch3[1] = latch3[0];
    }
    if (latch4[0].valid == 1) {
        latch4[1] = latch4[0];
    }

}