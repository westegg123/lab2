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
#define HLT 0xd4400000 // entire instr, not code
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

void clear_IF_ID_REGS() {
	CURRENT_REGS.IF_ID.PC = 0;
	CURRENT_REGS.IF_ID.instruction = 0;
}

void clear_ID_EX_REGS() {
	CURRENT_REGS.ID_EX.PC = 0;
	CURRENT_REGS.ID_EX.immediate = 0;
	CURRENT_REGS.ID_EX.primary_data_holder = 0;
	CURRENT_REGS.ID_EX.secondary_data_holder = 0;
}

void clear_EX_MEM_REGS() {
	CURRENT_REGS.EX_MEM.PC = 0;
	CURRENT_REGS.EX_MEM.ALU_result = 0;
	CURRENT_REGS.EX_MEM.data_to_write = 0;
}

void clear_MEM_WB_REGS() {
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
	parsed_instruction_holder INSTRUCTION_HOLDER = get_holder(CURRENT_REGS.ID_EX.instruction);
	// CPU_State NEXT_STATE = CURRENT_STATE;
	// INSTRUCTION = CURRENT_REGS.ID_EX.instruction;
	int WRITE_TO = -1;
	if (INSTRUCTION_HOLDER.format == 1) {
		WRITE_TO = INSTRUCTION_HOLDER.Rd;
	} else if (INSTRUCTION_HOLDER.format == 2) {
		WRITE_TO = INSTRUCTION_HOLDER.Rd;
	} else if (INSTRUCTION_HOLDER.format == 3) {
		WRITE_TO = INSTRUCTION_HOLDER.Rt;
	} else if (INSTRUCTION_HOLDER.format == 4) {
		printf("SOMETHING WEIRD HAPPENING - BR SHOULDNT WRITE BACK\n");
	} else if (INSTRUCTION_HOLDER.format == 5) {
		printf("SOMETHING WEIRD HAPPENING - CB SHOULDNT WRITE BACK\n");
	} else if (INSTRUCTION_HOLDER.format == 6) {
		WRITE_TO = INSTRUCTION_HOLDER.Rd;
	}

	if (WRITE_TO != -1) {
		CURRENT_STATE.REGS[WRITE_TO] = CURRENT_REGS.MEM_WB.ALU_result;
	}
}

void pipe_stage_mem() {
	clear_MEM_WB_REGS();
	parsed_instruction_holder INSTRUCTION_HOLDER = get_holder(CURRENT_REGS.ID_EX.instruction);
	// CPU_State NEXT_STATE = CURRENT_STATE;
	if (INSTRUCTION_HOLDER.format == 1) {
		printf ("SOMETHING WEIRD HAPPENING - R INSTR SHOULDNT GO TO MEM\n");
	} else if (INSTRUCTION_HOLDER.format == 2) {
		printf ("SOMETHING WEIRD HAPPENING - I INSTR SHOULDNT GO TO MEM\n");
	} else if (INSTRUCTION_HOLDER.format == 3) {
		// load
		if (INSTRUCTION_HOLDER.op == 0x7C2 || INSTRUCTION_HOLDER.op == 0x1C2 || INSTRUCTION_HOLDER.op == 0x3C2) {
			CURRENT_REGS.MEM_WB.fetched_data = mem_read_32(CURRENT_REGS.EX_MEM.ALU_result);
		} else /* store */{
			mem_write_32(CURRENT_REGS.EX_MEM.ALU_result, CURRENT_REGS.EX_MEM.data_to_write);
		}
	} else if (INSTRUCTION_HOLDER.format == 4) {
		printf("SOMETHING WEIRD HAPPENNING - BR SHOULDNT GO TO MEM\n");
	} else if (INSTRUCTION_HOLDER.format == 5) {
		printf("SOMETHING WEIRD HAPPENNING - CB SHOULDNT GO TO MEM\n");
	} else if (INSTRUCTION_HOLDER.format == 6) {
		printf("SOMETHING WEIRD HAPPENNING - MOVZ SHOULDNT GO TO MEM\n");
	}
	CURRENT_REGS.MEM_WB.ALU_result = CURRENT_REGS.EX_MEM.ALU_result;
}

void handle_add() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder + CURRENT_REGS.ID_EX.secondary_data_holder;
}

void handle_adds() {
	handle_add();
	CURRENT_STATE.FLAG_Z = (CURRENT_REGS.ID_EX.primary_data_holder + CURRENT_REGS.ID_EX.secondary_data_holder) == 0 ? 1 : 0;
	CURRENT_STATE.FLAG_N = (CURRENT_REGS.ID_EX.primary_data_holder + CURRENT_REGS.ID_EX.secondary_data_holder) < 0 ? 1 : 0;
}

void handle_and() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder & CURRENT_REGS.ID_EX.secondary_data_holder;	
}

void handle_ands() {
	handle_and();
	CURRENT_STATE.FLAG_Z = (CURRENT_REGS.ID_EX.primary_data_holder & CURRENT_REGS.ID_EX.secondary_data_holder) == 0 ? 1 : 0;
	CURRENT_STATE.FLAG_N = (CURRENT_REGS.ID_EX.primary_data_holder & CURRENT_REGS.ID_EX.secondary_data_holder) < 0 ? 1 : 0;
}

void handle_eor() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder ^ CURRENT_REGS.ID_EX.secondary_data_holder;	
}

void handle_orr() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder | CURRENT_REGS.ID_EX.secondary_data_holder;
}

void handle_lsl() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder << (0x3F - CURRENT_REGS.ID_EX.secondary_data_holder);
}

void handle_lsr() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder >> CURRENT_REGS.ID_EX.secondary_data_holder;
}

void handle_sub() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder - CURRENT_REGS.ID_EX.secondary_data_holder;		
}

void handle_subs() {
	handle_sub();
	CURRENT_STATE.FLAG_Z = (CURRENT_REGS.ID_EX.primary_data_holder - CURRENT_REGS.ID_EX.secondary_data_holder) == 0 ? 1 : 0;
	CURRENT_STATE.FLAG_N = (CURRENT_REGS.ID_EX.primary_data_holder - CURRENT_REGS.ID_EX.secondary_data_holder) < 0 ? 1 : 0;
}

void handle_br() {
	printf("havent handled branching yet\n");
}

void handle_mul() {
	CURRENT_REGS.EX_MEM.ALU_result = CURRENT_REGS.ID_EX.primary_data_holder * CURRENT_REGS.ID_EX.secondary_data_holder;		
}

// R INSTR EXECUTE STAGE
void pipe_stage_execute() {
	clear_EX_MEM_REGS();
	parsed_instruction_holder HOLDER = get_holder(CURRENT_REGS.ID_EX.instruction);
	CPU_State NEXT_STATE = CURRENT_STATE;
	if (HOLDER.format == 1) {
		if (HOLDER.opcode == 0x458 || HOLDER.opcode == 0x459) {
			handle_add();
		} else if (HOLDER.opcode == 0x558 || HOLDER.opcode == 0x559) {
			handle_adds();
		} else if (HOLDER.opcode == 0x450) {
			handle_and();
		} else if (HOLDER.opcode == 0x750) {
			handle_ands();
		} else if (HOLDER.opcode == 0x650) {
			handle_eor();
		} else if (HOLDER.opcode == 0x550) {
			handle_orr():
		} else if (HOLDER.opcode == 0x69B) {
			if (get_instruction_segment(10,15, CURRENT_REGS.ID_EX.instruction) == 0x3F) {
				handle_lsr();
		} 	else {
				handle_lsl();
			}
		} else if (HOLDER.opcode == 0x69A) {
			if (get_instruction_segment(10,15, CURRENT_REGS.ID_EX.instruction) != 0x3F) {
				handle_lsl();
		} 	else {
				handle_lsr();
			}	
		} else if (HOLDER.opcode == 0x658 || HOLDER.opcode == 0x659) {
			handle_sub();
		} else if (HOLDER.opcode == 0x758 || HOLDER.opcode == 0x759) {
			handle_subs();
		} else if (HOLDER.opcode == 0x6B0) {
			handle_br();
		} else if (HOLDER.opcode == 0x4D8) {
			handle_mul();
		}
	} else {
		printf("HAVENT HANDLED NON R INSTR YET\n");
	}
	// } else if (HOLDER.format == 3) {
	// 	if (HOLDER.op == 0x7C2 || HOLDER.op == 0x1C2 || HOLDER.op == 0x3C2) {
	// 		execute();
	// 		CURRENT_REGS.EX_MEM.ALU_result = NEXT_STATE.REGS[HOLDER.Rt];
	// 	} else {
	// 		if (HOLDER.format == 0x7C0) {
	// 			CURRENT_REGS.EX_MEM.ALU_result = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
	// 			CURRENT_REGS.EX_MEM.data_to_write = NEXT_STATE.REGS[HOLDER.Rt];
	// 		} else if (HOLDER.format == 0x1C0) {
	// 			CURRENT_REGS.EX_MEM.ALU_result = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
	// 			CURRENT_REGS.EX_MEM.data_to_write = get_memory_segment(0,7, NEXT_STATE.REGS[HOLDER.Rt]);
	// 		} else if (HOLDER.format == 0x3C0) {
	// 			CURRENT_REGS.EX_MEM.ALU_result = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
	// 			CURRENT_REGS.EX_MEM.data_to_write = get_memory_segment(0,15, NEXT_STATE.REGS[HOLDER.Rt]);
	// 		} else if (HOLDER.format == 0x5C0) {
	// 			CURRENT_REGS.EX_MEM.ALU_result = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
	// 			CURRENT_REGS.EX_MEM.data_to_write = get_memory_segment(0,31, NEXT_STATE.REGS[HOLDER.Rt]);
	// 		}
	// 	}
	// }
	// } else if (HOLDER.format == 4) {
	// 	;
	// } else if (HOLDER.format == 5) {
	// 	;
	// } else if (HOLDER.format == 6) {
	// 	;
	// }
	}
}

void pipe_stage_decode() {
	clear_ID_EX_REGS();
	parsed_instruction_holder INSTRUCTION_HOLDER = get_holder(CURRENT_REGS.IF_ID.instruction);
	CURRENT_REGS.ID_EX.PC = CURRENT_REGS.IF_ID.PC;
	CURRENT_REGS.ID_EX.instruction = CURRENT_REGS.IF_ID.instruction;

	if (INSTRUCTION_HOLDER.format == 1) { // R
		CURRENT_REGS.ID_EX.primary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
		CURRENT_REGS.ID_EX.secondary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rm];
		if (INSTRUCTION_HOLDER.opcode == 0x69B) {
			CURRENT_REGS.ID_EX.secondary_data_holder = INSTRUCTION_HOLDER.shamt;
		} else if (INSTRUCTION_HOLDER.opcode == 0x69A) {
			CURRENT_REGS.ID_EX.secondary_data_holder = get_instruction_segment(16,21, INSTRUCTION);
		}

	// } else if (INSTRUCTION_HOLDER.format == 2) { // I
	// 	CURRENT_REGS.ID_EX.primary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
	// 	CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.ALU_immediate;

	// } else if (INSTRUCTION_HOLDER.format == 3) { // D
	// 	CURRENT_REGS.ID_EX.primary_data_holder = CURRENT_STATE.REGS[INSTRUCTION_HOLDER.Rn];
	// 	CURRENT_REGS.ID_EX.immediate = sign_extend(INSTRUCTION_HOLDER.DT_address, 26, 2);

	// } else if (INSTRUCTION_HOLDER.format == 4) { // B
	// 	CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.BR_address;

	// } else if (INSTRUCTION_HOLDER.format == 5) { // CB
	// 	CURRENT_REGS.ID_EX.primary_data_holder = sign_extend(INSTRUCTION_HOLDER.COND_BR_address, 19, 2);

	// } else if (INSTRUCTION_HOLDER.format == 6) { // IM/IW
	// 	CURRENT_REGS.ID_EX.immediate = INSTRUCTION_HOLDER.MOV_immediate;
	// }
	}
}

// OK
void pipe_stage_fetch() {
	clear_IF_ID_REGS();
	CURRENT_REGS.IF_ID.instruction = mem_read_32(CURRENT_STATE.PC);
	CURRENT_REGS.IF_ID.PC = CURRENT_STATE.PC;
	CURRENT_STATE.PC += 4;
}
