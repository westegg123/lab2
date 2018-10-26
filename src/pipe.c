/*
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 */

#include "pipe.h"
#include "shell.h"
#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


/* global pipeline state */
CPU_State CURRENT_STATE;

Pipeline_Regs CURRENT_REGS;

/************************************ CONSTANTS ************************************/
/* 
 * NOT INCLUDED: CBNZ, CBZ, MOVZ, B, B.COND
 * 
 */
#define HLT 0xd4400000
#define ADD 0x458 //need to add one to handle both cases
#define ADDI 0x488 //need to add one to handle both cases
#define ADDS 0x558 //need to add one to handle both cases
#define ADDIS 0x588 //need to add one to handle both cases
#define AND 0x450
#define ANDS 0x750
#define EOR 0x650
#define ORR 0x550
#define LDUR_64 0x7C2
#define LDUR_32 0x5C2
#define LDURB 0x1C2
#define LDURH 0x3C2
#define LSL	0x69B //IMMEDIATE VERSION
#define LSR 0x69A //IMMEDIATE VERSION
#define STUR 0x7C0
#define STURB 0x1C0
#define STURH 0x3C0
#define STURW 0x5C0
#define SUB 0x658 //need to add one to handle both cases
#define SUBI 0x688 //need to add one to handle both cases
#define SUBS 0x758 //need to add one to handle both cases
#define SUBIS 0x788
#define MUL 0x4D8
#define BR 0x6B0

/************************************ END OF CONSTANTS ************************************/

/************************************ HELPERS ************************************/

void clear_ID_REGS(){
	CURRENT_REGS.IF_ID.PC = 0;
	CURRENT_REGS.IF_ID.instruction = 0;
}

void clear_EX_REGS(){
	CURRENT_REGS.ID_EX.PC = 0;
	CURRENT_REGS.ID_EX.immediate = 0;
	CURRENT_REGS.ID_EX.primary_data_holder = 0;
	CURRENT_REGS.ID_EX.secondary_data_holder = 0;
}

void clear_MEM_REGS(){
	CURRENT_REGS.EX_MEM.PC = 0;
	CURRENT_REGS.EX_MEM.ALU_result = 0;
	CURRENT_REGS.EX_MEM.data_to_write = 0;
}

void clear_WB_REGS(){
	CURRENT_REGS.MEM_WB.fetched_data = 0;
	CURRENT_REGS.MEM_WB.ALU_result = 0;
}


/************************************ END OF HELPERS ************************************/

void pipe_init() {
    memset(&CURRENT_STATE, 0, sizeof(CPU_State));
    CURRENT_STATE.PC = 0x00400000;
}

void pipe_cycle() {
	pipe_stage_wb();
	pipe_stage_mem();
	pipe_stage_execute();
	pipe_stage_decode();
	pipe_stage_fetch();
}

void pipe_stage_wb() {
	if (INSTRUCTION_HOLDER.format == 1) {

	} else if (INSTRUCTION_HOLDER.format == 2) {

	} else if (INSTRUCTION_HOLDER.format == 3) {

	} else if (INSTRUCTION_HOLDER.format == 4) {

	} else if (INSTRUCTION_HOLDER.format == 5) {

	} else if (INSTRUCTION_HOLDER.format == 6) {

	}

}

void pipe_stage_mem() {
	clear_WB_REGS();

	if (INSTRUCTION_HOLDER.format == 4) {

	} else if (INSTRUCTION_HOLDER.format == 5) {

	} else if (INSTRUCTION_HOLDER.format == 6) {

	}
}

void pipe_stage_execute() {
	clear_MEM_REGS();
	if (INSTRUCTION_HOLDER.format == 1) {

	} else if (INSTRUCTION_HOLDER.format == 2) {

	} else if (INSTRUCTION_HOLDER.format == 3) {

	} else if (INSTRUCTION_HOLDER.format == 4) {

	} else if (INSTRUCTION_HOLDER.format == 5) {

	} else if (INSTRUCTION_HOLDER.format == 6) {

	}
}

void pipe_stage_decode() {
	clear_EX_REGS();
	parsed_instruction_holder INSTRUCTION_HOLDER = get_holder(CURRENT_REGS.IF_ID.instruction);
	CURRENT_REGS.ID_EX.PC = CURRENT_REGS.IF_ID.PC;

	if (INSTRUCTION_HOLDER.format == 1) { // R
		CURRENT_REGS.ID_EX.primary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
		CURRENT_REGS.ID_EX.secondary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rm];

	} else if (INSTRUCTION_HOLDER.format == 2) { // I
		CURRENT_REGS.ID_EX.primary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
		CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.ALU_immediate

	} else if (INSTRUCTION_HOLDER.format == 3) { // D
		CURRENT_REGS.ID_EX.CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
		CURRENT_REGS.ID_EX.immediate = sign_extend(INSTRUCTION_HOLDER.DT_address, 26, 2);

	} else if (INSTRUCTION_HOLDER.format == 4) { // B
		CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.BR_address;

	} else if (INSTRUCTION_HOLDER.format == 5) { // CB
		INSTRUCTION_HOLDER.sign_extend(INSTRUCTION_HOLDER.COND_BR_address, 19, 2);

	} else if (INSTRUCTION_HOLDER.format == 6) { // IM/IW
		CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.MOV_immediate;
	}
}

void pipe_stage_fetch() {
	clear_ID_REGS();
	CURRENT_REGS.IF_ID.instruction = mem_read_32(CURRENT_STATE.PC);
	CURRENT_REGS.IF_ID.PC = CURRENT_STATE.PC;
	CURRENT_STATE.PC += 4;
}
