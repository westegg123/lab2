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

/*********************** HELPERS ************************************/


	
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
}

void pipe_stage_mem() {
}

void pipe_stage_execute() {
}

void pipe_stage_decode() {
	parsed_instruction_holder INSTRUCTION_HOLDER = get_holder(CURRENT_REGS.IF_ID.instruction);
	CURRENT_REGS.ID_EX.PC = CURRENT_REGS.IF_ID.PC;
}

void pipe_stage_fetch() {
	CURRENT_REGS.IF_ID.instruction = mem_read_32(CURRENT_STATE.PC);
	CURRENT_REGS.IF_ID.PC = CURRENT_STATE.PC;
	CURRENT_STATE.PC += 4;
}
