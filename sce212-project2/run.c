/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

#define REGS CURRENT_STATE.REGS
#define PC CURRENT_STATE.PC

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
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/

#define ZERO_EX(INST) (0x0000ffff & (unsigned int)IMM(&INST))
#define BRANCH_ADD(INST) (SIGN_EX(IMM(&INST)) << 2)
#define JUMP_ADD(INST) ((0xf000000 & PC) | (TARGET(&INST) << 2))

 void process_instruction()
 {
    instruction INST;

    if(PC == (MEM_TEXT_START+(NUM_INST * 4)))
        {
            RUN_BIT = FALSE;
            return;
        }

    INST = *get_inst_info(PC);

    switch(OPCODE(&INST))
    {
        case 0x00: //Type R

            switch(FUNC(&INST))
            {                 
               case 0x21: REGS[RD(&INST)] = REGS[RS(&INST)] + REGS[RT(&INST)]; break; //ADDU
               case 0x23: REGS[RD(&INST)] = REGS[RS(&INST)] - REGS[RT(&INST)]; break; //SUBU
               case 0x24: REGS[RD(&INST)] = REGS[RS(&INST)] & REGS[RT(&INST)]; break; //AND
	           case 0x25: REGS[RD(&INST)] = REGS[RS(&INST)] | REGS[RT(&INST)]; break; //OR
               case 0x27: REGS[RD(&INST)] = ~ (REGS[RS(&INST)] | REGS[RT(&INST)]); break; //NOR
	           case 0x2b: REGS[RD(&INST)] = REGS[RS(&INST)] < REGS[RT(&INST)] ? 1 : 0; break; //SLTU
	           case 0x00: REGS[RD(&INST)] = REGS[RT(&INST)] << SHAMT(&INST); break; //SLL
	           case 0x02: REGS[RD(&INST)] = REGS[RT(&INST)] >> SHAMT(&INST); break; //SRL
               case 0x08: PC = REGS[RS(&INST)] - 4; break; //JR
            }  
            break;

        //Type I
        case 0x9: REGS[RT(&INST)] = REGS[RS(&INST)] + SIGN_EX(IMM(&INST)); break; //ADDIU
        case 0xc: REGS[RT(&INST)] = REGS[RS(&INST)] & ZERO_EX(INST); break; //ANDI
        case 0xf: REGS[RT(&INST)] = (IMM(&INST) << 16); break; //LUI	
        case 0xd: REGS[RT(&INST)] = REGS[RS(&INST)] | ZERO_EX(INST); break; //ORI
        case 0xb: REGS[RT(&INST)] = REGS[RS(&INST)] < SIGN_EX(IMM(&INST)) ? 1 : 0; break; //SLTIU
        case 0x23: REGS[RT(&INST)] = mem_read_32(REGS[RS(&INST)]+SIGN_EX(IMM(&INST))); break; //LW
        case 0x2b: mem_write_32(REGS[RS(&INST)]+SIGN_EX(IMM(&INST)), REGS[RT(&INST)]); break; //SW
        case 0x4: if (REGS[RS(&INST)] == REGS[RT(&INST)]) PC = (PC+BRANCH_ADD(INST)); break; //BEQ
        case 0x5: if (REGS[RS(&INST)] != REGS[RT(&INST)]) PC = (PC+BRANCH_ADD(INST)); break; //BNE

        //TYPE J
        case 0x2: PC = JUMP_ADD(INST) - 4; break; //J
        case 0x3: REGS[31] = PC+8; PC = JUMP_ADD(INST) - 4; break; //JAL

        default:
            printf("Not available instruction\n");
            assert(0);
    }
    PC = PC+4;

    return;
 }