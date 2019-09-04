/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   util.h                                                    */
/*   Adapted from Computer Architecture@KAIST                  */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Basic Information */
#define MEM_TEXT_START	0x00400000
#define MEM_TEXT_SIZE	0x00100000
#define MEM_DATA_START	0x10000000
#define MEM_DATA_SIZE	0x00100000
#define MIPS_REGS	32
#define BYTES_PER_WORD	4
#define PIPE_STAGE	5

#define IF_STAGE 	0
#define ID_STAGE	1
#define EX_STAGE	2
#define MEM_STAGE	3
#define WB_STAGE	4

typedef struct inst_s {
    short opcode;

    /*R-type*/
    short func_code;

    union {
        /* R-type or I-type: */
        struct {
	    unsigned char rs;
	    unsigned char rt;

	    union {
	        short imm;

	        struct {
		    unsigned char rd;
		    unsigned char shamt;
		} r;
	    } r_i;
	} r_i;
        /* J-type: */
        uint32_t target;
    } r_t;

    uint32_t value;
    
    //int32 encoding;
    //imm_expr *expr;
    //char *source_line;
} instruction;


/* You may add pipeline registers that you require */
typedef struct CPU_State_Struct {
	uint32_t PC;			/* program counter for the IF stage*/
	uint32_t REGS[MIPS_REGS];	/* register file */
	uint32_t REGS_LOCK[MIPS_REGS];	/* register lock to support stalls 
					   Lock registers when data is not ready*/
	
	uint32_t PIPE[PIPE_STAGE];	/* PC being executed at each stage*/
	uint32_t PIPE_STALL[PIPE_STAGE];
	
	//IF_ID_latch
    instruction IF_ID_INST; // change data type
	uint32_t IF_ID_NPC;

	//ID_EX_latch
	uint32_t ID_EX_NPC; // pc+4
	uint32_t ID_EX_REG1; // read register1
	uint32_t ID_EX_REG2; // read register2
	uint32_t ID_EX_IMM; // immediate 필드 -> change data type
	unsigned char ID_EX_DEST; // rd (r-type) or rt(i-type) 번호 저장
	/* 추가 시작 */
	/* for forwarding */
	unsigned char ID_EX_RS; // rs 번호
	unsigned char ID_EX_RT; // rt 번호
	unsigned char ID_EX_SHMAT; // SLL SRL
	/* for control signal */
	char id_ALUNot;
	char id_Branch_beq;
	char id_MemRead;
	char id_MemtoReg;
	char id_ALUOp;
	char id_MemWrite;
	char id_ALUSrc;
	char id_RegWrite;
	char id_Branch_bne;
	

	int load_use;

	//EX_MEM_latch
	uint32_t EX_MEM_NPC; // branch 주소 계산 위함
	uint32_t EX_MEM_ALU_OUT; // ALU OUTPUT
	uint32_t EX_MEM_W_VALUE; // 메모리 들어갈 값
	uint32_t EX_MEM_BR_TARGET; // 계산된 브랜치 주소
	uint32_t EX_MEM_BR_TAKE; // 브랜치 되었을 때 알리는 것 ZERO TEST 결과
	unsigned char EX_MEM_DEST;
	/* for control signal */
	char ex_MemRead;
	char ex_MemWrite;
	char ex_RegWrite;
	char ex_MemtoReg;


	//MEM_WB_latch
	uint32_t MEM_WB_NPC; //
	uint32_t MEM_WB_ALU_OUT; // ALU 연산 결과 저장
	uint32_t MEM_WB_MEM_OUT; // MEM 참조 데이터 저장
	uint32_t MEM_WB_BR_TAKE; //
	unsigned char MEM_WB_DEST; // 2 CYCLE 차이나는 INST 포워딩을 위함 && sw일때 거기있는 값을 메모리에 저장시켜야하므로 데이터가 들어있는 reg의 번호를 mem/wb로 넘긴다.
	/* for control signal */
	char mem_RegWrite;
	char mem_MemtoReg;

	//To choose right PC
	uint32_t IF_PC; // PC+4
	uint32_t JUMP_PC; // J-TYPE TARGET 주소
	uint32_t BRANCH_PC; // BRANCH된 주소

    int isj;
	
} CPU_State;

typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;

/* For PC * Registers */
extern CPU_State CURRENT_STATE;

/* For Instructions */
extern instruction *INST_INFO;
extern int NUM_INST;

/* For Memory Regions */
extern mem_region_t MEM_REGIONS[2];

/* For Execution */
extern int RUN_BIT;	/* run bit */
extern int FETCH_BIT;	/* instruction fetch bit */ // 플러시할때 사용해보자.
extern int INSTRUCTION_COUNT;
extern uint64_t MAX_INSTRUCTION_NUM;
extern uint64_t CYCLE_COUNT;

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		fromBinary(const char *s);
uint32_t	mem_read_32(uint32_t address);
void		mem_write_32(uint32_t address, uint32_t value);
void		cycle();
void		run();
void		go();
void		mdump(int start, int stop);
void		rdump();
void		pdump();
void		init_memory();
void		init_inst_info();

/* YOU IMPLEMENT THIS FUNCTION in the run.c file */
void process_instruction();
void IF_Stage();
void ID_Stage();
void EX_Stage();
void MEM_Stage();
void WB_Stage();
void FLUSH_IF();
void FLUSH_ID();
void FLUSH_EX();
void Forwarding_Unit();
void ALU_OPERATION();
#endif
