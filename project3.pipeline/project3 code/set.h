#include <stdio.h>


//function initialize
int reg(int reg1, int reg2);
int control(int opcode, int funct);
int ALU(int data1, int data2);
int mem(int address, int wirteData);
int ALUcontrol(int simm);
int ForwardA(int rs, int  regDst1, int regDst2);
int ForwardB(int rt, int  regDst1, int regDst2);

void fetch();
void decode();
void execute();
void memaccess();
void wb();
void Switch();

void flush_f(int regdata);
void Flush1();
void Flush2();

int AlwaysTakenPredict(int pc);
int BTFN(int pc);
int OneBitBranchPredict(int pc);
int TwoBitBranchPredict(int pc);
int TwoLevelPredict(int GHR, int pc);
int LocalTwoLevelPredict(int pc);


void AlwaysTakenPredictCheck(int b_address, int b_taken, int bpc);
void AlwaysNotTakenPredictCheck(int b_address, int b_taken, int bpc);
void BTFNPredictCheck(int b_address, int b_taken, int bpc);
void OneBitBranchPredictCheck(int b_address, int b_taken, int bpc);
void TwoBitBranchPredictCheck(int b_address, int b_taken, int bpc);
void TwoLevelPredictCheck(int b_address, int b_taken, int bpc);
void LocalTwoLevelPredictCheck(int b_address, int b_taken, int bpc);




//initialized construction
int Reg[32];//32bit register
unsigned int inst;
int pc ;//pc 
int memory[0x400000];//16mb memory
int BTB[0x400000];
int B_Taken[0x400000];
int GHR ;
int BPHT[0x400000];

//global vars
int becond;// result of the brach alu
int clock ;//to check the clock cycle
int sp ;
int predict_correction ;
int mis_predict ;
int AlwaysTakenAddress ;
int AlwaysTakenpc ;
int memoryops ;
int regops ;
int jumpinsts ;
int branchinsts ;
int numOfInsts ;

struct EX {
    //ex
    int RegDst;
    int ALUSrc;
    int ALUOp;
    int jr;
    int jal;
    int branch;
    int zero;
};

struct M {
    //m

    int memoryWrite;
    int memoryRead;
    int PCSrc1;
};

struct WB {
    //wb
    int memtoReg;
    int PCSrc2;
    int RegWrite;
    int j_and_l_toReg;

};


struct IF_ID {
    int valid;
    int inst;
    int pc_4;
};

struct ID_EX {
    struct EX ex;
    struct M m;
    struct WB wb;

    int valid;
    int pc_4;
    int Rdata1;
    int Rdata2;
    int simm;
    int rs;
    int rd;
    int rt;
    int shamt;
    int imm;
};


struct EX_MEM {
    struct M m;
    struct WB wb;

    int valid;
    int pc_4;
    int becond;
    int aluResult;
    int writeData;
    int regDst;
    int addresultpc;
    int jrs;
    int simm;

};



struct MEM_WB {
    struct WB wb;

    int valid;
    int mReadData;
    int aluResult;
    int regDst;
    int jrs;
    int pc_4;

};




struct IF_ID latch1[2];
struct ID_EX latch2[2];
struct EX_MEM latch3[2];
struct MEM_WB latch4[2];


