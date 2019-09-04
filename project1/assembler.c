#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

/*******************************************************
 * Function Declaration
 *
 *******************************************************/
char *change_file_ext(char *str);
void dec2bin(int n, int size,FILE* stream);

/*******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/
#define MAX_LENGTH 40
#define MAX_NUM 100

typedef struct data // data1, data2, data3, array2
{
	char name[MAX_LENGTH];
	long int data;
	long int data_address;
}data; // 데이터 라벨이름, 데이터값, 데이터의 주소값

typedef struct label // label1 -0 ,label2 -1,label3 -2,label4-3,label5-4,loop-5,exit-6;
{
	char label_name[MAX_LENGTH];
	int pc_num;
}label; // 라벨 이름, 라벨의 주소(pc값 이용)

data data_list[] = { {"data1",0,0x0},{"data2",0,0x0},{"data3",0,0x0},{"array2",0,0x0} };

label label_list[] = { {"lab1",0x0},{"lab2",0x0},{"lab3",0x0},{"lab4",0x0},{"lab5",0x0},{"loop",0x0},{"exit",0x0} };

int
main(int argc, char *argv[])
{
    FILE *input, *output;
    char *filename;
        long int binary2data[100]; // 데이터를 이진수로 변환한것을 저장
	char x[MAX_LENGTH][MAX_LENGTH]; // 파일로부터 읽어오고 바로 저장하는 배열
	char *y[MAX_NUM] = { NULL, }; // 자른 문자열 저장할 배열
	char* ptr; // 자를 때 쓸 포인터
	char* ptrc;
	char *inst[10] = { NULL, }; // 자른 문자열 저장
	char token[] = " ,$()\t\n"; // 토큰
	int i = 0;
	int k = 0;
	int j = 0;
	int t = 0;
	int g = 0;
	int num = 0; // 데이터 번호, 갯수
	int globalpointer = 0x1000000; // 글로벌 포인터
	int pc = 0x3ffffc; // pc주소 초기
	int inst_num = 0; // 명령어 수
	int temp=0;
	int offset=0;
	int j_field = 0;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if(change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }
    output = fopen(filename, "w");
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }
    while (!feof(input))
	{
		fgets(x[t],1000, input); //파일에서 받아와서
		ptrc = strtok(x[t], token); // 자르고 
		while (ptrc != NULL) // 계속자르고
		{
			y[g] = ptrc;
			g++;
			ptrc = strtok(NULL, token);
		}
		t++; // 행 늘리면서 계속자르고
	}
	// 명령어에서 참조할 데이터 저장 시작
	for (j = 0; y[j] != NULL; j++) // 지역변수 "j" 사용
	{
		if (strcmp(y[j], "data1:") == 0) // data 1: <=> data_list[0]
		{
			if (strncmp(y[j + 2], "0x", 2) == 0) // 16진수인 경우
			{
				data_list[0].data = strtol(y[j + 2], NULL, 16); // 데이터 집어넣고 
			}
			else // 10진수인 경우
			{
				data_list[0].data = strtol(y[j + 2], NULL, 10);
			}
			data_list[0].data_address = globalpointer; // 주소 저장후 처음 나온게 아니라면 +4시킨다.
			globalpointer += 4;
		}
		else if (strcmp(y[j], "data2:") == 0) // data 2: <=> data_list[1]
		{
			if (strncmp(y[j + 2], "0x", 2) == 0)
			{
				data_list[1].data = strtol(y[j + 2], NULL, 16);
			}
			else
			{
				data_list[1].data = strtol(y[j + 2], NULL, 10);
			}
			data_list[1].data_address = globalpointer; // 주소 저장후 처음 나온게 아니라면 +4시킨다.
			globalpointer += 4;
		}
		else if (strcmp(y[j], "data3:") == 0) // data 3: <=> data_list[2]
		{
			if (strncmp(y[j + 2], "0x", 2) == 0)
			{
				data_list[2].data = strtol(y[j + 2], NULL, 16);
			}
			else
			{
				data_list[2].data = strtol(y[j + 2], NULL, 10);
			}
			data_list[2].data_address = globalpointer; // 주소 저장후 처음 나온게 아니라면 +4시킨다.
			globalpointer += 4;
		}
		else if (strcmp(y[j], "array:") == 0)
		{
			if (strcmp(y[j + 1], ".word") == 0 && strcmp(y[j + 3], ".word") == 0 && strcmp(y[j + 5], ".word") == 0)
			{
				globalpointer += 12;
			}
		}
		else if (strcmp(y[j], "array2:") == 0) // array2: <=> data_list[3]
		{
			if (strncmp(y[j + 2], "0x", 2) == 0)
			{
				data_list[3].data = strtol(y[j + 2], NULL, 10);
			}
			else
			{
				data_list[3].data = strtol(y[j + 2], NULL, 10);
			}
			data_list[3].data_address = globalpointer; // 주소 저장후 처음 나온게 아니라면 +4시킨다.
			globalpointer += 4;
		}
	}
	// 2진수로 변경한 데이터, 데이터 갯수
	for (j = 0; y[j] != NULL; j++)
	{
		if (strcmp(y[j], ".word") == 0)
		{
			if (strncmp(y[j + 1], "0x", 2) == 0)
			{	
				binary2data[num] = strtol(y[j + 1], NULL, 16);
			}
			else
			{
				binary2data[num] = strtol(y[j + 1], NULL, 10);
			}
			num++;
		}
	}
	// pc값 갱신
	for (j = 0; y[j] != NULL; j++)
	{
		if (strcmp(y[j], "addiu") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "addu") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "and") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "andi") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "beq") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "bne") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "j") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "jal") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "jr") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "lui") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "lw") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "nor") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "or") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "ori") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "sltiu") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "sltu") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "sll") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "srl") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "sw") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "subu") == 0)
		{
			pc += 4;
			inst_num++;
		}
		else if (strcmp(y[j], "la") == 0)
		{
			for (int r = 0; r < 4; r++)
			{
				if (strcmp(y[j + 2], data_list[r].name) == 0) // la $8 data1로 쪼개질 것이므로 la가 j번째면 label은 j+2번째 저장
				{
					if (data_list[r].data_address > 0x1000000)
					{
						pc += 8; // lui, ori로 쪼개지는 경우
						inst_num += 2;
					}
					else
					{
						pc += 4; // lui 하나인경우
						inst_num++;
					}
				}
			}
		}
		// 라벨에 값 집어 넣기
		else if (strcmp(y[j], "lab1:") == 0) // label1 -0 
		{
			label_list[0].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "lab2:") == 0) // label2 - 1
		{
			label_list[1].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "lab3:") == 0) // label3 -2 
		{
			
			label_list[2].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "lab4:") == 0) // label4 - 3
		{
			label_list[3].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "lab5:") == 0) // label5 - 4
		{
			label_list[4].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "loop:") == 0) // loop - 5
		{
			label_list[5].pc_num = pc + 4;
		}
		else if (strcmp(y[j], "exit:") == 0) // exit - 6
		{
			label_list[6].pc_num = pc + 4;
		}
	}

	fseek(input, 0, SEEK_SET);
	pc = 0x3ffffc;
	dec2bin(4*inst_num,32,output);
	dec2bin(4*num, 32,output);
	//while (!feof(input))
	while(fgets(x[i],40,input)!=NULL)
	{
		// fgets(x[i],40,input);
		ptr = strtok(x[i],token); // 처음만 잘라서 어떤 명령인지 구분한다.

		if (strcmp(ptr,"addiu") == 0) // addiu
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			} //strtok에서 받은거 저장해야됨 그래야 변환
			dec2bin(0x9,6,output); // op
			dec2bin(atoi(inst[2]),5, output); // rs
			dec2bin(atoi(inst[1]),5, output); // rt
			if (strncmp(inst[3], "0x", 2) == 0)
			{
				dec2bin(strtol(inst[3], NULL, 16), 16, output); // immediate
			}
			else dec2bin(strtol(inst[3], NULL, 10), 16, output); // immediate
			k = 0; // k 초기화
			pc += 4;
		}
		else if (strcmp(ptr,"addu") == 0)  // addu
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output);//rs
			dec2bin(atoi(inst[3]), 5, output);//rt
			dec2bin(atoi(inst[1]), 5, output);//rd
			dec2bin(0, 5, output);// shmat
			dec2bin(0x21,6, output);//funct 
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr,"and") == 0) // and
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); //op
			dec2bin(atoi(inst[2]), 5, output);//rs
			dec2bin(atoi(inst[3]), 5, output);//rt
			dec2bin(atoi(inst[1]), 5, output);//rd
			dec2bin(0,5, output);// shmat
			dec2bin(0x24, 6, output);//funct 
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "andi") == 0) // andi
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			} //strtok에서 받은거 저장해야됨 그래야 변환
			dec2bin(0xc, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); // rt
			if (strncmp(inst[3], "0x", 2) == 0)
			{
				dec2bin(strtol(inst[3], NULL, 16), 16, output); // immediate
			}
			else dec2bin(strtol(inst[3], NULL, 10), 16, output); // immediate
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "beq") == 0) // beq
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x4, 6, output); // op
			dec2bin(atoi(inst[1]), 5, output); // rs
			dec2bin(atoi(inst[2]), 5, output); // rt
			temp = pc+8;
			for (int r = 0; r < 7; r++)
			{
				if (strcmp(inst[3], label_list[r].label_name) == 0)
				{
					offset = (label_list[r].pc_num - temp) / 4; // offset 계산
				}
			}
			dec2bin(offset, 16, output); // immediate - offset
			k=0;
			temp = 0;
			offset = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "bne") == 0) // bne
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x5, 6, output); // op
			dec2bin(atoi(inst[1]), 5, output); // rs
			dec2bin(atoi(inst[2]), 5, output); // rt
			temp = pc+8;
			for (int r = 0; r < 7; r++)
			{
				if (strcmp(inst[3], label_list[r].label_name) == 0)
				{
					offset = (label_list[r].pc_num - temp) / 4; // offset 계산
				}
			}
			dec2bin(offset, 16, output); // immediate - offset
			k = 0;
			temp = 0;
			offset = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "j") == 0) // j
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x2, 6, output); // op
			for (int r = 0; r < 7; r++)
			{
				if (strcmp(inst[1], label_list[r].label_name) == 0)
				{
					j_field = (label_list[r].pc_num) / 4; // 해당 라벨주소에서 / 4 한값이 들어간다.
				}
			}
			dec2bin(j_field, 26, output);
			k = 0;
			j_field = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "jal") == 0) // jal
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x3, 6, output); // op
			for (int r = 0; r < 7; r++)
			{
				if (strcmp(inst[1], label_list[r].label_name) == 0)
				{
					j_field = (label_list[r].pc_num) / 4; // 해당 라벨주소에서 / 4 한값이 들어간다.
				}
			}
			dec2bin(j_field, 26, output);
			k = 0;
			j_field = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "la") == 0) // la
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			for (int r = 0; r < 4; r++)
			{
				if (strcmp(inst[2], data_list[r].name) == 0) // la $8 data1
				{
					if (data_list[r].data_address > 0x1000000)
					{
						// lui && upper 16bit
						dec2bin(0xf, 6, output); // op
						dec2bin(0, 5, output); // rs
						dec2bin(atoi(inst[1]), 5, output); //rt
						dec2bin((data_list[r].data_address >> 12), 16, output); // upper 16 bit
						// ori && lower 16bit
						dec2bin(0xd, 6, output); // op
						dec2bin(atoi(inst[1]), 5, output); // rs
						dec2bin(atoi(inst[1]), 5, output); // rt
						dec2bin((data_list[r].data_address & 0x0000ffff), 16, output); // lower 16 bit
						pc += 8; // lui, ori로 쪼개지는 경우
					}
					else
					{
						// lui && upper 16bit
						dec2bin(0xf, 6, output); // op
						dec2bin(0, 5, output); // rs
						dec2bin(atoi(inst[1]), 5, output); //rt
						dec2bin((data_list[r].data_address >> 12), 16, output); // upper 16 bit
						pc += 4; // lui 하나인경우	
					}
				}
			}
			k = 0;
		}
		else if (strcmp(ptr, "jr") == 0) // jr
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(atoi(inst[1]), 5, output); // rs
			dec2bin(0, 5, output); // rt
			dec2bin(0, 5, output); // rd
			dec2bin(0, 5, output); // shamt
			dec2bin(0x08, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "lui") == 0) //lui
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0xf, 6, output); // op
			dec2bin(0, 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); //rt
			if (strncmp(inst[2], "0x", 2) == 0)
			{
				dec2bin(strtol(inst[2], NULL, 16), 16, output); // immediate
			}
			else dec2bin(strtol(inst[2], NULL, 10), 16, output); // immediate
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "lw") == 0) // lw
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x23, 6, output); // op
			dec2bin(atoi(inst[3]), 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); // rt
			dec2bin(atoi(inst[2]), 16, output); // offset
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "nor") == 0) // nor
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[3]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(0, 5, output); // shmat
			dec2bin(0x27, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "or") == 0) // or
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output);
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[3]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(0, 5, output); // shmat
			dec2bin(0x25, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "ori") == 0) //ori
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0xd, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); // rt
			if (strncmp(inst[3], "0x", 2) == 0)
			{
				dec2bin(strtol(inst[3], NULL, 16), 16, output); // immediate
			}
			else dec2bin(strtol(inst[3], NULL, 10), 16, output); // immediate
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "sltiu") == 0) // sltiu
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0xb, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); // rt
			if (strncmp(inst[3], "0x", 2) == 0)
			{
				dec2bin(strtol(inst[3], NULL, 16), 16, output); // immediate
			}
			else dec2bin(strtol(inst[3], NULL, 10), 16, output); // immediate
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "sltu") == 0) // sltu
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[3]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(0, 5, output); // shmat
			dec2bin(0x2b, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "sll") == 0) // sll
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(0, 5, output); //rs
			dec2bin(atoi(inst[2]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(atoi(inst[3]), 5, output); // shmat
			dec2bin(0x00, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "srl") == 0) // srl
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(0, 5, output); //rs
			dec2bin(atoi(inst[2]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(atoi(inst[3]), 5, output); // shmat
			dec2bin(0x02, 6, output); // funct
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "sw") == 0) // sw
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0x2b, 6, output); // op
			dec2bin(atoi(inst[3]), 5, output); // rs
			dec2bin(atoi(inst[1]), 5, output); // rt
			dec2bin(atoi(inst[2]), 16, output); // offset
			k = 0;
			pc += 4;
		}
		else if (strcmp(ptr, "subu") == 0) //subu
		{
			while (ptr != NULL)
			{
				inst[k] = ptr;
				k++;
				ptr = strtok(NULL, token);
			}
			dec2bin(0, 6, output); // op
			dec2bin(atoi(inst[2]), 5, output); // rs
			dec2bin(atoi(inst[3]), 5, output); // rt
			dec2bin(atoi(inst[1]), 5, output); // rd
			dec2bin(0, 5, output); // shmat
			dec2bin(0x23, 6, output); // funct
			k = 0;
			pc += 4;
		}
		i++;
	}
	if (num != 0)
	{
		for (int q = 0; q < num; q++)
		{
			dec2bin(binary2data[q], 32, output);
		}
	}
	fprintf(output, "\n");

    fclose(input);
    fclose(output);
    exit(EXIT_SUCCESS);

}


/*******************************************************
 * Function: change_file_ext
 *
 * Parameters:
 *  char
 *      *str: a raw filename (without path)
 *
 * Return:
 *  return NULL if a file is not an assembly file.
 *  return empty string
 *
 * Info:
 *  This function reads filename and converst it into
 *  object extention name, *.o
 *
 *******************************************************/
char
*change_file_ext(char *str)
{
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0)) {
        return NULL;
    }

    str[strlen(str) - 1] = 'o';
    return "";
}

void dec2bin(int n, int size,FILE* stream)
{
	int decimal = n;
	int binary[32] = { 0, };
	int position = 0;
	if (n >= 0)
	{
		while (1)
		{
			binary[position] = decimal % 2;
			decimal = decimal / 2;
			position++;
			if (decimal == 0) break;
		}
		for (int i = size - 1; i >= 0; i--) fprintf(stream,"%d", binary[i]);
	}
	else
	{
		decimal++;
		decimal *= -1;
		while (1)
		{
			binary[position] = decimal % 2;
			decimal = decimal / 2;
			position++;
			if (decimal == 0) break;
		}
		for (int i = size - 1; i >= 0; i--) fprintf(stream,"%d",!(binary[i]));
	}
}
