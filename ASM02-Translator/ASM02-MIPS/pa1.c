/**********************************************************************
* Copyright (c) 2019
*  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/* To avoid security error on Visual Studio */
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING FROM THIS LINE ******       */
#define MAX_NR_TOKENS	32	/* Maximum length of tokens in a command */
#define MAX_TOKEN_LEN	64	/* Maximum length of single token */
#define MAX_ASSEMBLY	256 /* Maximum length of assembly string */

typedef unsigned char bool;
#define true	1
#define false	0
/*          ****** DO NOT MODIFY ANYTHING UP TO THIS LINE ******      */
/*====================================================================*/


/***********************************************************************
* translate
*
* DESCRIPTION
*   Translate assembly represented in @tokens[] into a MIPS instruction.
* This translate should support following 13 assembly commands
*
*	- add
*	- addi
*	- sub
*	- and
*	- andi
*	- or
*	- ori
*	- nor
*	- lw
*	- sw
*	- sll
*	- srl
*	- sra
*
* RETURN VALUE
*   Return a 32-bit MIPS instruction
*
*/

char* RSDtoB(int num)
{
	int result = 0;
	int changed = 0;
	char* binary = malloc(6);
	for (int i = 1; num > 0; i = i * 10)
	{
		result = num % 2;
		changed = changed + (result * i);
		num = num / 2;
	}
	binary[6] = NULL;
	sprintf(binary, "%05d", changed);          
	return binary;
}

int flag;

char* IDtoB(int num, int flag)
{
	int result = 0;
	long long int changed = 0;
	char* binary = (char*)malloc(17);
	for (long long int i = 1, j = 0; j <15; i = i * 10, j++)
	{
		result = num % 2;
		changed = changed + (result * i);
		num = num / 2;
	}
	changed = changed + flag * 1000000000000000;
	binary[17] = NULL;
	sprintf(binary, "%016lld", changed);
	return binary;
}



static unsigned int translate(int nr_tokens, char *tokens[])
{
	/* TODO:
	* This is an example MIPS instruction. You should change it accordingly.
	*/
	
	struct r_set {
		char *r_opcode;
		char *rs;
		char *rt;
		char *rd;
		char *shamt;
		char *r_funct;
	};
	struct s_set{
		char *s_opcode;
		char *rs;
		char *rt;
		char *rd;
		char *shamt;
		char *s_funct;
	};
	struct i_set{
		char *i_opcode;
		char *i_rs;
		char *i_rt;
		char *cons;
	};
	
	
	char *Type[13] = { "add", "sub", "and", "or" , "nor", "sll", "srl", "sra" , "addi", "andi", "ori", "lw", "sw" };                                                    
	const char *RName[32] = { "zero","at" ,"v0" ,"v1" ,"a0" ,"a1" ,"a2" ,"a3" ,"t0" ,"t1" ,"t2" ,"t3", "t4", "t5" ,"t6", "t7", "s0", "s1", 
"s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1","gp", "sp", "fp", "ra" };
	char *result[33];
	char *regi[5];
	struct r_set rset; 
	struct s_set sset; 
	struct i_set iset;
	int Final;

	for (int i = 0; i < 13; i++)
	{
		int type = strcmp(Type[i], tokens[0]);
		if (type == 0)
		{
			if (i <= 4)
			{
				for (int j = 1; j < 4; j++)
				{
					for (int k = 0; k < 32; k++)
					{
						if (strcmp(tokens[j], RName[k]) == 0)
						{
							regi[j] = RSDtoB(k);
							break;
						}
					}
				}

				rset.r_opcode = "000000";

				rset.rs = regi[2];
				rset.rt = regi[3];
				rset.rd = regi[1];

				rset.shamt = "00000";

				if (i == 0) rset.r_funct = "100000";
				if (i == 1) rset.r_funct = "100010";
				if (i == 2) rset.r_funct = "100100";
				if (i == 3) rset.r_funct = "100101";
				if (i == 4) rset.r_funct = "100111";
				
				sprintf(result, "\n%s%s%s%s%s%s\n", rset.r_opcode, rset.rs, rset.rt, rset.rd, rset.shamt ,rset.r_funct);
				printf("%s", result);
			}
			else if (5 <= i && i <= 7)
			{
				for (int j = 1; j < 3; j++)
				{
					for (int k = 0; k < 32; k++)
					{
						if (strcmp(tokens[j], RName[k]) == 0)
						{
							regi[j] = RSDtoB(k);
							break;
						}
					}
				}
					int num = strtol(tokens[3], NULL, 0);
					regi[3] = RSDtoB(num);
			
				sset.s_opcode = "000000";

				sset.rs = "00000";

				sset.rt = regi[2];
				sset.rd = regi[1];
				sset.shamt = regi[3];

				if (i == 5) sset.s_funct = "000000";
				if (i == 6) sset.s_funct = "000010";
				if (i == 7) sset.s_funct = "000011";
				
				sprintf(result, "\n%s%s%s%s%s%s\n", sset.s_opcode, sset.rs, sset.rt, sset.rd, sset.shamt, sset.s_funct);
				printf("%s", result);
			}
		
			else
			{
				if (i == 8) iset.i_opcode = "001000";
				if (i == 9) iset.i_opcode = "001100";
				if (i == 10) iset.i_opcode = "001101";
				if (i == 11) iset.i_opcode = "100011";
				if (i == 12) iset.i_opcode = "101011";

				for (int j = 1; j < 3; j++)
				{
					for (int k = 0; k < 32; k++)
					{
						if (strcmp(tokens[j], RName[k]) == 0)
						{
							regi[j] = RSDtoB(k);
							break;
						}
					}
				}
				int num = strtol(tokens[3], NULL, 0);
				if (num >= 0)
				{
					if (num < 32768)
					{
						num = 32767 & num;
						flag = 0;
						regi[3] = IDtoB(num, flag);
					}
					else
					{
					num = 32767 & num;
					flag = 1;
					regi[3] = IDtoB(num, flag);
					}
				}
				else
				{
					num = num & 32767;
					flag = 1;
					regi[3] = IDtoB(num, flag);
				}
				

				iset.i_rs = regi[2];
				iset.i_rt = regi[1];
				iset.cons = regi[3];

				sprintf(result, "\n%s%s%s%s\n", iset.i_opcode, iset.i_rs, iset.i_rt, iset.cons);
				printf("%s", result);
			}
		}
	}

	Final = strtol(result, NULL, 2);

	return Final;
}



/***********************************************************************
* parse_command
*
* DESCRIPTION
*  Parse @assembly, and put each assembly token into @tokens[] and the number of
*  tokes into @nr_tokens. You may use this implemention or your own from PA0.
*
* A assembly token is defined as a string without any whitespace (i.e., *space*
* and *tab* in this programming assignment). For exmaple,
*   command = "  add t1   t2 s0 "
*
* then, nr_tokens = 4, and tokens is
*   tokens[0] = "add"
*   tokens[1] = "t0"
*   tokens[2] = "t1"
*   tokens[3] = "s0"
*
* You can assume that the input string is all lowercase for testing.
*
* RETURN VALUE
*  Return 0 after filling in @nr_tokens and @tokens[] properly
*
*/
static bool __is_separator(char *c)
{
	char *separators = " \t\r\n,.";

	for (size_t i = 0; i < strlen(separators); i++) {
		if (*c == separators[i]) return true;
	}

	return false;
}
static int parse_command(char *assembly, int *nr_tokens, char *tokens[])
{
	char *curr = assembly;
	int token_started = false;
	*nr_tokens = 0;

	while (*curr != '\0') {
		if (__is_separator(curr)) {
			*curr = '\0';
			token_started = false;
		}
		else {
			if (!token_started) {
				tokens[*nr_tokens] = curr;
				*nr_tokens += 1;
				token_started = true;
			}
		}
		curr++;
	}

	return 0;
}



/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING FROM THIS LINE ******       */

/***********************************************************************
* The main function of this program.
*/
int main(int argc, char * const argv[])
{
	char assembly[MAX_ASSEMBLY] = { '\0' };
	FILE *input = stdin;

	if (argc > 1) {
		input = fopen(argv[1], "r");
		if (!input) {
			fprintf(stderr, "No input file %s\n", argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (input == stdin) {
		printf("*********************************************************\n");
		printf("*          >> SCE212 MIPS translator  v0.01 <<          *\n");
		printf("*                                                       *\n");
		printf("*                                       .---.           *\n");
		printf("*                           .--------.  |___|           *\n");
		printf("*                           |.------.|  |=. |           *\n");
		printf("*                           || >>_  ||  |-- |           *\n");
		printf("*                           |'------'|  |   |           *\n");
		printf("*                           ')______('~~|___|           *\n");
		printf("*                                                       *\n");
		printf("*                                   Fall 2019           *\n");
		printf("*********************************************************\n\n");
		printf(">> ");
	}

	while (fgets(assembly, sizeof(assembly), input)) {
		char *tokens[MAX_NR_TOKENS] = { NULL };
		int nr_tokens = 0;
		unsigned int machine_code;

		for (size_t i = 0; i < strlen(assembly); i++) {
			assembly[i] = tolower(assembly[i]);
		}

		if (parse_command(assembly, &nr_tokens, tokens) < 0)
			continue;

		machine_code = translate(nr_tokens, tokens);

		fprintf(stderr, "0x%08x\n", machine_code);

		if (input == stdin) printf(">> ");
	}

	if (input != stdin) fclose(input);

	return EXIT_SUCCESS;
}