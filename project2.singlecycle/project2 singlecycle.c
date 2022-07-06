#include <stdio.h>
//function initialize
int reg(int reg1, int reg2, int writeReg);
int control(int opcode);
int ALU(int data1, int data2);
int mem(int address, int wirteData);
void fetch();
void execute();
void decode();



//initialized construction
int Reg[32];//32bit register
int ret;
int inst;
int pc = 0;//pc 
int inst;
int memory[0x400000];//16mb memory


struct inst_ {
    int inst;
    int opcode;
    int rs;
    int rt;
    int rd;
    signed int imm;
    int simm;//signextend immediate number
    int jtarget;
    int funct;
    int j_r;//jr jtarget value
    int shamt;
};
struct inst_ instr;

struct IF_ID{
    int inst[2];
    int pc[2];

};
struct IF_ID la1


struct ID_EX{
    int pc[2];
    int readData1[2];
    int readData2[2];
    int signextend[2];
    int funct[2];
    int rd[2];
    int wb;//signals
    int m;
    int ex;
};
struct ID_EX la2

struct EX_MEM{
    int after_addpc[2];
    int becond[2];
    int result[2];
    int rt_data[2];
    int rd[2];

    int wb;
    int m;


};
struct EX_MEM la3


struct MEM_WB{
    int memdata[2];
    int aluresult[2];
    int rd[2];
    int wb;

};
struct MEM_WB la4

//control signals
int RegDst = 0;
int ALUSrc = 0;
int RegWrite = 0;
int memoryWrite = 0;
int memoryRead = 0;
int memtoReg = 0;
int pcadd = 0;
int ALUOp = 0;
int PCSrc1 = 0;
int PCSrc2 = 0;
int PC = 0;
int branch = 0;
int j_and_l = 0;
int j_and_l_toReg = 0;
int jr = 0;
int sp = 0x1000000;
int zeroex = 0;



//global vars
int becond;// result of the brach alu
int clock = 0;//to check the clock cycle



int main(int argc, char* argv[]) {
    //handling input args
    char* input_file = "simple.bin";//file name which want to open
    FILE* fp;
    int ret;
    int fin = 0;
    int cycle = 0;

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


    //begin execution loop
    while (1) {
       
        if (pc == -1)break;
        //in the loop..

       
        fetch();
        decode();
        execute();
        //mem();
        //wb();


        if (clock > 23372000) {//check the clockcyle.
            printf("clcokcycle: %d\n ", clock);

        }
        else {
            if (clock % 1000 == 0) {
                printf("clcokcycle %d\n ", clock);

            }
        }


        //initialize the control signals by opcode.->explain

        //printf("--------------------------------------------------\n");
        //printf("pc[0x%x] \n", pc);

       // printf("instruction: 0x%x\n", inst);
       // printf("opcode: 0x%x\n", instr.opcode);


        if (j_and_l) {//if instruction is jal than add 8 in pc
            pc = pc + 4;
        }
        else {
            pc = pc + 8;
        }

        if (instr.opcode == 0x2 || instr.opcode == 0x3) {// shift 2bits and add the current upper pc 4bit to use jump address->explain
            int tempj = instr.jtarget << 2;
            instr.jtarget = (tempj + (pc & (0xf << 28)));

        }


        //updatapc  ->pc to hex -> explain
        if (PCSrc2) {//pc update depends on branch taken => result of the branch alu
            pc = (instr.simm << 2) + pc;
        }
        else if (PCSrc1) {//pc updata depends on jump
            pc = instr.jtarget;
        }
        else if (jr) {//if instruction is jump register than updata the pc for register value
            pc = instr.j_r;
            jr = 0;
        }

    }

    printf("R[2]:0x%x", Reg[2]);//print out result (result will be store in the register number 2)
    //end of exeution
    return 0;
}

void fetch(){
     //fetch
    memset(&instr, 0, sizeof(instr));
    inst = memory[pc / 4];
}

void decode(){
    //decode
        instr.inst = inst;
        instr.opcode = (inst >> 26) & 0x3f;  // get some opcode bit from the instruction -> explain

        if (instr.opcode == 0x0) {// r type instruction
            instr.rs = (inst >> 21) & 0x1f;
            instr.rt = (inst >> 16) & 0x1f;
            instr.rd = (inst >> 11) & 0x1f;
            instr.shamt = (inst >> 6) & 0x1f;
            instr.funct = inst & 0x3f;
        }
        else {// not r type instruction
            if (instr.opcode == 0x2 || instr.opcode == 0x3) {//jump and jump and link( j type instruction)
                instr.jtarget = inst & 0x3ffffff;
            }
            else {//i type instrution
                instr.rs = (inst >> 21) & 0x1f;
                instr.rt = (inst >> 16) & 0x1f;
                instr.imm = inst & 0xffff;
            }
        }
        control(instr.opcode);

}

void execute(){
    //execute
        if (RegDst == 1) {// r-type
            reg(instr.rs, instr.rt, instr.rd);
        }
        else if (RegDst == 0) {//not r-type


            reg(instr.rs, instr.rt, instr.rt);
        }
        else if (RegDst == 2) {//initialize the R[31] for the register destination
            reg(instr.rs, instr.rt, 31);

        }
}

void 

int control(int opcode) {//function to initialize the control signal->explain
    RegDst = opcode == 0;//r type instrution 
    if (opcode == 0x3) {// jump and link
        RegDst = 2;
    }
    ALUSrc = (opcode != 0) && (opcode != 0x4) && (opcode != 0x5);
    memtoReg = opcode == 0x23;
    RegWrite = (opcode != 0x2b) && (opcode != 0x4) && (opcode != 0x5) && (opcode != 0x2) && (opcode != 0x3);
    memoryRead = opcode == 0x23;//load word
    memoryWrite = opcode == 0x2b;//store word
    PCSrc1 = (opcode == 0x2) || (opcode == 0x3); //jump or not
    PCSrc2 = (opcode == 0x4) && (opcode == 0x5);//bcond statisfied
    branch = (opcode == 0x4) || (opcode == 0x5);//branch
    j_and_l = (opcode != 0x3);
    j_and_l_toReg = (opcode != 0x3);
    jr = (opcode == 0x0) && (instr.funct == 0x08);//jr mux signal
    ALUOp = instr.opcode;

    if (opcode == 0xd) {//or immediate
        zeroex = 1;
    }
}

int reg(int reg1, int reg2, int writeReg) {

    int data1 = 0;
    int data2 = 0;

    //get data from the register
    data1 = Reg[reg1];
    data2 = Reg[reg2];
    instr.j_r = data1;
    if (writeReg == 31) {
        Reg[writeReg] = pc;
    }

    if (RegWrite) {//register update 
        Reg[writeReg] = ALU(data1, data2);
    }
    else {//memory updata
        ALU(data1, data2);
    }
    return 0;
}
//execution
int ALU(int data1, int data2) {
    int result = 0;
    int temp = instr.imm;

    //opcode
    if (temp >> 15 == 0) {//imm sign extend
        instr.simm = 0xffffffff & instr.imm;
    }
    else {
        instr.simm = (0xffff << 16) | instr.imm;
    }

    if (zeroex) {//zeroextend of ori
        int zeroextend = instr.imm;
        instr.simm = 0xffffffff & instr.imm;


    }



    if (ALUSrc) {//not R type and not branch
        switch (ALUOp)
        {
        case 0x8://addi
            result = data1 + instr.simm;
            break;

        case 0x9://addiu
            result = data1 + instr.simm;
            break;

        case 0x2://jump here we just initlized the control signals
            break;

        case 0x3://jump and link
            break;
        case 0x2b://sw
            result = data1 + instr.simm;
            break;
        case 0x23:
            result = data1 + instr.simm;
            break;
        case 0xa://slti
            if (data1 < instr.simm)result = 1;
            break;
        case 0xd: //ori     
            result = data1 | instr.simm;
            break;
        case 0xf://lui
            result = instr.imm << 16;
            break;
        }

    }
    else {//R type and branch
        switch (ALUOp)
        {
        case 0x4://beq
            if (data1 == data2) {
                becond = 1;
                PCSrc2 = becond & branch;

            }
            else {
                becond = 0;
                PCSrc2 = becond & branch;
            }
            break;
        case 0x5://bne
            if (data1 != data2) {
                becond = 1;
                PCSrc2 = becond & branch;

            }
            else {
                becond = 0;
                PCSrc2 = becond & branch;
            }
            break;
        case 0x0:       //r type instruction  
            if (instr.funct == 0x20) {//add
                result = data1 + data2;
            }
            else if (instr.funct == 0x21) {//addu
                result = data1 + data2;
            }
            else if (instr.funct == 0x08) {//jump register
                result = 0;
            }
            else if (instr.funct == 0x2a) {//slt
                if (data1 < data2)result = 1;

            }
            else if (instr.funct == 0x22) {//sub
                result = data1 - data2;
            }
            else if (instr.funct == 0x02) {//srl
                int temp = data2 >> instr.shamt;
                result = temp;
            }
            else if (instr.funct == 0x00) {//shift left logical       
                int temp = data2 << instr.shamt;
                result = temp;
            }
            else if (instr.funct == 0x23) {//subtract unsigned
                result = data1 - data2;
            }
            break;
        }
    }


    int result_m = 0;
    result_m = mem(result, data2);
    if (memtoReg) {//return the value depends on memtoReg signal
        return result_m;
    }
    else {
        return result;
    }
    //writeback


}



int mem(int address, int wirteData) {//memory
    if (memoryWrite == 1) {//sw
        memory[address / 4] = wirteData;
        return 0;
    }
    else if (memoryRead == 1) {//lw   
        if (memtoReg) {
            return memory[address / 4];
        }
        else {
            return address;
        }
    }
    else if (memoryRead == 0 && memoryRead == 0) {
        return 0;
    }
    else if (j_and_l_toReg == 0) {
        return pc;
    }
}


/*
-execution-
fetch
decode
execute
memory
wirteback
update
*/