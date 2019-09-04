/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from Computer Architecture@KAIST                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
	char op[7]={'\0',};
    char rs[6]={'\0',};
    char rt[6]={'\0',};
    char rd[6]={'\0',};
    char shmat[6]={'\0',};
    char func[7]={'\0',};
    char imm[17]={'\0',};
    char target[27]={'\0',};
	char move2mem[33]={'\0',};
	strcpy(move2mem, buffer); // 길이 32 문자열 읽어와서
	int inst = fromBinary(move2mem); // 2진수로 변환시킨다
	mem_write_32(MEM_TEXT_START + index, inst); // 메모리에 적재 pc 부터 시작
	strncpy(op, buffer, 6); // op 코드 분리
	instr.opcode = (short)fromBinary(op); // 2진수 변환 후 구조체에 집어넣고
		switch (instr.opcode)
		{
		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xf:		//(0x001111)LUI	
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0x23:		//(0x100011)LW
		case 0x2b:		//(0x101011)SW
		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE
			strncpy(rs, buffer + 6, 5); // rs 분리
			instr.r_t.r_i.rs = (unsigned char)fromBinary(rs); // 이진수 변환 자료형 맞추고
			strncpy(rt, buffer + 11, 5); // rt 분리
			instr.r_t.r_i.rt = (unsigned char)fromBinary(rt); // 이진수 변환
			strncpy(imm, buffer + 16, 16); // imm 분리
			instr.r_t.r_i.r_i.imm = (short)fromBinary(imm); // 2진수 변환
			instr.value = 1;
			break;
		case 0x0:
			strncpy(rs, buffer + 6, 5); // rs 분리
			instr.r_t.r_i.rs = (unsigned char)fromBinary(rs); // 이진수 변환
			strncpy(rt, buffer + 11, 5); // rt 분리
			instr.r_t.r_i.rt = (unsigned char)fromBinary(rt); // 이진수 변환
			strncpy(rd, buffer + 16, 5); // rd 분리
			instr.r_t.r_i.r_i.r.rd = (unsigned char)fromBinary(rd); // 이진수 변환
			strncpy(shmat, buffer + 21, 5); // shmat분리
			instr.r_t.r_i.r_i.r.shamt = (unsigned char)fromBinary(shmat); // 이진수 변환
			strncpy(func, buffer + 26, 6); // funct 분리
			instr.func_code = (short)fromBinary(func); // 이진수 변환
			instr.value = 1;
			break;
		case 0x2:		//(0x000010)J
		case 0x3:		//(0x000011)JAL
			strncpy(target, buffer + 6, 26); // target분리
			instr.r_t.target = fromBinary(target); // 2진수 변환
			instr.value = 1;
			break;
		default:
			printf("Not available instruction\n");
		}
    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	char movetomem[33];
	strcpy(movetomem, buffer); // 파일에서 읽어서 배열에 잠시 복사
	int data = fromBinary(movetomem); // 이진수로 변환하고
	mem_write_32(MEM_DATA_START + index, data); // 메모리에 적재 global pointer 부터
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
