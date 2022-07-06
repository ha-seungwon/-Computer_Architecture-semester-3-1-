#include <stdio.h>
#include <stdbool.h>

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


int Cachefill(int address, int way, int index, int tag);
int Cache4fill(int address, int way, int index, int tag);
int Cache8fill(int address, int way, int index, int tag);
int DMCachefill(int address, int index, int tag);
int FACachefill(int address,int index, int tag);


int FASWCache(int address, int data);
int DMSWCache(int address, int data);
int SWCache(int address, int data);
int SWCache4(int address, int data);
int SWCache8(int address, int data);


int FALWCache(int address);
int DMLWCache(int address);
int LWCache(int address);
int LWCache4(int address);
int LWCache8(int address);

int replace(int way,int index);

//initialized construction
int Reg[32];//32bit register
unsigned int inst;
int pc ;//pc 
int memory[0x400000];//16mb memory
int BTB[0x400000];
int B_Taken[0x400000];
int GHR ;
int instmemory[0x400000];
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
int cachereplace;


int cachemiss;
int cachehit;
int coldMiss;
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
    unsigned int aluResult;
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


struct DMCacheline {
    unsigned int tag : 19; // among 32 bit, use only a few bits
    unsigned int valid : 1;
    int data[16];//64byte array
};

struct DMCache {
    struct DMCacheline cacheline[128];
};

struct FACacheline {

    unsigned int tag : 26; // among 32 bit, use only a few bits
    unsigned int valid : 1;
    unsigned int dirty : 1;
    unsigned int sca : 1;
    int data[16];//64byte array
};



struct Cache2line {

    unsigned int tag : 20; // among 32 bit, use only a few bits
    unsigned int valid : 1;
    unsigned int dirty : 1;
    unsigned int sca : 1;
    int data[16];//64byte array
};
struct Cacheway {
    struct Cache2line cacheline[64];

};







struct Cache4line {

    unsigned int tag : 21; // among 32 bit, use only a few bits
    unsigned int valid : 1;
    unsigned int dirty : 1;
    unsigned int sca : 1;
    int data[16];//64byte array
};

struct Cache4way {
    struct Cache4line cacheline[32];

};

struct Cache8line {
    
    unsigned int tag : 22; // among 32 bit, use only a few bits
    unsigned int valid : 1;
    unsigned int dirty : 1;
    unsigned int sca : 1;
    int data[16];//64byte array
};
struct Cache8way {
    struct Cache8line cacheline[16];

};




struct IF_ID latch1[2];
struct ID_EX latch2[2];
struct EX_MEM latch3[2];
struct MEM_WB latch4[2];

struct DMCache directmapcache;
struct FACacheline FAcache[128];
struct Cacheway cache2way[2];
struct Cache4way cache4way[4];
struct Cache8way cache8way[8];


struct FACachequeue {
    int queue[128];
};

struct Cache2queue {
    int queue[2];
};
struct Cache4queue {
    int queue[4];
};
struct Cache8queue {
    int queue[8];
};

struct FACachequeue FAcachequeue;
struct Cache2queue cache2queue[64];
struct Cache4queue cache4queue[32];
struct Cache8queue cache8queue[16];


struct lru {
    int oldest;
};


struct lru FAcachelru;
struct lru cache2lru[64];
struct lru cache4lru[32];
struct lru cache8lru[16];


//#define MAX 2
//#define MAX 4  // 
//#define MAX 8
#define MAX 128
#define MappingType 8//0: fully 1:dm 2:2way 4:4way 8:8way
#define RPType 0//0: random 1: fifo 2:lru 3;sca
int front ;
int rear ;

void init(int queue[], int data, int rear, int front);
int delete(int queue, int rear, int front);
