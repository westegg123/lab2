/*
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 */

#include "pipe.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


// PIPELINE REGISTER STRUCTS
typedef struct IF_ID_REGS {
	uint64_t PC, instruction;
} IF_ID_REGS;

typedef struct ID_EX_REGS {
	uint64_t PC, immediate, primary_data_holder, secondary_data_holder;
} ID_EX_REGS;

typedef struct EX_MEM_REGS {
	uint64_t PC, ALU_result, data_to_write;
} EX_MEM_REGS;

typedef struct MEM_WB_REGS {
	uint64_t fetched_data, ALU_result;
} MEM_WB_REGS;

typedef struct Pipeline_Regs {
	IF_ID_REGS IF_ID;
	ID_EX_REGS ID_EX;
	EX_MEM_REGS EX_MEM;
	MEM_WB_REGS MEM_WB;
} Pipeline_Regs;
// END PIPELINE REGISTER STRUCTS

/* global pipeline state */
CPU_State CURRENT_STATE;

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
}

void pipe_stage_fetch() {
}
