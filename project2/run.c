/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from Computer Architecture@KAIST                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: isRtype                                          */
/*                                                             */
/* Purpose: Process one R type instruction                     */
/*                                                             */
/***************************************************************/
/*
void isRtype(instruction* inst)
{
	if (FUNC(inst) == 0x21) 
	{     //(100001) ADDU
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] + CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x24) 
	{     //(100100) AND
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] & CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x27) 
	{        //(100111) NOR
		CURRENT_STATE.REGS[RD(inst)] = ~(CURRENT_STATE.REGS[RS(inst)] | CURRENT_STATE.REGS[RT(inst)]);
	}
	else if (FUNC(inst) == 0x25) 
	{        //(100101) OR
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] | CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x2b) 
	{        //(101011) SLTU
		if (CURRENT_STATE.REGS[RS(inst)] < CURRENT_STATE.REGS[RT(inst)]) 
		{
			CURRENT_STATE.REGS[RD(inst)] = 1;
		}
	}
	else if (FUNC(inst) == 0x00) 
	{        //(000000) SLL
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RT(inst)] << SHAMT(inst);
	}
	else if (FUNC(inst) == 0x02) 
	{        //(000010) SRL
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RT(inst)] >> SHAMT(inst);
	}
	else if (FUNC(inst) == 0x23) 
	{       //(100011) SUBU
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] - CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x8) 
	{        //(001000) JR
		JUMP_INST(CURRENT_STATE.REGS[RS(inst)]);
	}
	else 
	{
		printf("This instruction is not R_type\n");
	}
}
*/
/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction() // INST_INFO -> process + reg write // Halt의 기능도 여기서 줘야한다 i=100 // util.h 참고
{

instruction *inst = get_inst_info(CURRENT_STATE.PC);

if (inst->value != 1) // halt condition 
{
	RUN_BIT = FALSE; // run bit = 0
	return; // program go heaven
}
int flag = 0;
CURRENT_STATE.PC += 4; // already pc  0x400000 used now +4 use and update 

switch (OPCODE(inst)) // classify op code
{
case 0x9:		//(001001)ADDIU
	CURRENT_STATE.REGS[RT(inst)] = CURRENT_STATE.REGS[RS(inst)] + IMM(inst); // rs + Imm = rt
	break;
case 0xc:		//(001100)ANDI
	CURRENT_STATE.REGS[RT(inst)] = CURRENT_STATE.REGS[RS(inst)] & IMM(inst); // rs & Imm = rt
	break;
case 0xf:		//(001111)LUI	
	CURRENT_STATE.REGS[RT(inst)] = (IMM(inst) << 16); // rt = upper 16 bit of imm
	break;
case 0xd:		//(001101)ORI
	CURRENT_STATE.REGS[RT(inst)] = CURRENT_STATE.REGS[RS(inst)] | IMM(inst); // rt = rs | imm
	break;
case 0xb:		//(001011)SLTIU
	if (CURRENT_STATE.REGS[RS(inst)] < IMM(inst)) // if rs<imm then, rt =1
	{
		CURRENT_STATE.REGS[RT(inst)] = 1;
	}
	else // if rs>imm then rt=0
	{
		CURRENT_STATE.REGS[RT(inst)] = 0;
	}
	break;
case 0x23:		//(100011)LW
	CURRENT_STATE.REGS[RT(inst)] = mem_read_32(CURRENT_STATE.REGS[RS(inst)] + IMM(inst)); // 예제 보면 lui로 global pointer시작값 넣고 lw함 즉, memread(rs+imm)
	break;
case 0x2b:		//(101011)SW
	mem_write_32(CURRENT_STATE.REGS[RS(inst)] + IMM(inst), CURRENT_STATE.REGS[RT(inst)]); // lw반대 memwrite
	break;
case 0x4:		//(000100)BEQ
	if (CURRENT_STATE.REGS[RS(inst)] == CURRENT_STATE.REGS[RT(inst)]) 
	{
		JUMP_INST(CURRENT_STATE.PC + (IMM(inst) << 2)); // (PC)+IMM(offset*4)
	}
	break;
case 0x5:		//(000101)BNE
	if (CURRENT_STATE.REGS[RS(inst)] != CURRENT_STATE.REGS[RT(inst)]) 
	{
		JUMP_INST(CURRENT_STATE.PC + (IMM(inst) << 2)); // (PC)+IMM(offset*4)
	}
	break;
	//TYPE R
case 0x0:		//(000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
	flag = 1;
	break;
	//TYPE J
case 0x2:		//(000010)J
	JUMP_INST(TARGET(inst) << 2); // jump ~!
	break;
case 0x3:		//(000011)JAL
	CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4; // 31's reg play ra reg you must see example
	JUMP_INST(TARGET(inst) << 2); // jump ~!
	break;
default:
	printf("Not available instruction\n");
}
if(flag==1)
{
	if (FUNC(inst) == 0x21) 
	{     //(100001) ADDU
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] + CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x24) 
	{     //(100100) AND
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] & CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x27) 
	{        //(100111) NOR
		CURRENT_STATE.REGS[RD(inst)] = ~(CURRENT_STATE.REGS[RS(inst)] | CURRENT_STATE.REGS[RT(inst)]);
	}
	else if (FUNC(inst) == 0x25) 
	{        //(100101) OR
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] | CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x2b) 
	{        //(101011) SLTU
		if (CURRENT_STATE.REGS[RS(inst)] < CURRENT_STATE.REGS[RT(inst)]) 
		{
			CURRENT_STATE.REGS[RD(inst)] = 1;
		}
	}
	else if (FUNC(inst) == 0x00) 
	{        //(000000) SLL
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RT(inst)] << SHAMT(inst);
	}
	else if (FUNC(inst) == 0x02) 
	{        //(000010) SRL
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RT(inst)] >> SHAMT(inst);
	}
	else if (FUNC(inst) == 0x23) 
	{       //(100011) SUBU
		CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)] - CURRENT_STATE.REGS[RT(inst)];
	}
	else if (FUNC(inst) == 0x8) 
	{        //(001000) JR
		JUMP_INST(CURRENT_STATE.REGS[RS(inst)]);
	}
	else 
	{
		printf("This instruction is not R_type\n");
	}
}
}
