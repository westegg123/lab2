#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "pipe.h"
#include "helper.h"
#include "helper.c"
// GLOBALS
uint64_t INSTRUCTION;

uint32_t get_instruction_segment_sim(int start_bit, int end_bit) {
	get_instruction_segment(start_bit, end_bit, INSTRUCTION);
}

parsed_instruction_holder HOLDER;
// END GLOBALS

void clear_holder() {
	HOLDER.opcode = 0;
	HOLDER.format = 0;
	HOLDER.Rm = 0;
	HOLDER.shamt = 0;
	HOLDER.Rn = 0;
	HOLDER.Rd = 0;
	HOLDER.Rt = 0;
	HOLDER.ALU_immediate = 0;
	HOLDER.DT_address = 0;
	HOLDER.op = 0;
	HOLDER.BR_address;
	HOLDER.COND_BR_address = 0;
	HOLDER.MOV_immediate = 0;
} 

// EXECUTE SUBROUTINES
extern CPU_State NEXT_STATE;
// ADD 
void handle_add() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] + NEXT_STATE.REGS[HOLDER.Rm];
}

// ADDI
void handle_addi() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.ALU_immediate;
}

// ADDS
void handle_adds() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] + NEXT_STATE.REGS[HOLDER.Rm];
	NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[HOLDER.Rn] + NEXT_STATE.REGS[HOLDER.Rm]) < 0 ? 1 : 0;
	NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[HOLDER.Rn] + NEXT_STATE.REGS[HOLDER.Rm]) == 0 ? 1 : 0; 
}

// ADDIS
void handle_addis() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.ALU_immediate;
	NEXT_STATE.FLAG_N = (NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.ALU_immediate) < 0 ? 1 : 0;
	NEXT_STATE.FLAG_Z = (NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.ALU_immediate) == 0 ? 1 : 0;
}

// CBNZ
void handle_cbnz() {
	if (NEXT_STATE.REGS[HOLDER.Rt] != 0) {
		uint64_t offset = sign_extend(HOLDER.COND_BR_address, 19, 2);
		NEXT_STATE.PC = NEXT_STATE.PC + offset;
	} else {
		NEXT_STATE.PC += 4;
	}
}

// CBZ
void handle_cbz() {
	if(NEXT_STATE.REGS[HOLDER.Rt] == 0) {
		uint64_t offset = sign_extend(HOLDER.COND_BR_address, 19, 2);
		NEXT_STATE.PC = NEXT_STATE.PC + offset;
	} else {
		NEXT_STATE.PC += 4;
	}
}

// AND
void handle_and() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] & NEXT_STATE.REGS[HOLDER.Rm];
}

// ANDS
void handle_ands() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] & NEXT_STATE.REGS[HOLDER.Rm];
	NEXT_STATE.FLAG_N = NEXT_STATE.REGS[HOLDER.Rn] & NEXT_STATE.REGS[HOLDER.Rm] < 0 ? 1 : 0;
	NEXT_STATE.FLAG_Z = NEXT_STATE.REGS[HOLDER.Rn] & NEXT_STATE.REGS[HOLDER.Rm] == 0 ? 1 : 0;
}

// EOR
void handle_eor() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] ^ NEXT_STATE.REGS[HOLDER.Rm];
}

// ORR
void handle_orr() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] | NEXT_STATE.REGS[HOLDER.Rm];
}

// LDUR - 64 
// Pulls 64 bits
void handle_ldur64() {
	NEXT_STATE.REGS[HOLDER.Rt] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
}

// LDUR - 32
// Pulls 32 bits
void handle_ldur32() {
	NEXT_STATE.REGS[HOLDER.Rt] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
}

// LDURB
// Pulls 8 bits
void handle_ldurb() {
	NEXT_STATE.REGS[HOLDER.Rt] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
}

// LDURH
// Pulls 16 bits
void handle_ldurh() {
	NEXT_STATE.REGS[HOLDER.Rt] = NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address;
}

// LSL (Immediate)
void handle_lsl() {
	NEXT_STATE.REGS[HOLDER.Rd] = (uint64_t)NEXT_STATE.REGS[HOLDER.Rn] << (0x3F - HOLDER.shamt); // NOT SURE WHY
	printf("64 reg val %lx\n", (uint64_t)(NEXT_STATE.REGS[HOLDER.Rn] << (0x3F - HOLDER.shamt)));
	printf("32 reg val %lx\n", NEXT_STATE.REGS[HOLDER.Rn] << (0x3F - HOLDER.shamt));
	printf("actual shamt: %d\n", (0x3F - HOLDER.shamt));
	printf("HANDLING LSL (IMMEDIATE)\n");
}

// LSR (Immediate)
void handle_lsr() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] >> get_instruction_segment_sim(16,21); // NOT SURE WHY. 
	printf("actual shamt: %d\n", get_instruction_segment_sim(16,21));
	printf("HANDLING LSR (IMMEDIATE)\n");
}

// MOVZ (NO SHIFTING NECESSARY FOR PURPOSES OF LAB 1)
void handle_movz() {
	NEXT_STATE.REGS[HOLDER.Rd] = HOLDER.MOV_immediate;
	//printf("MOV_immediate: %x\n", HOLDER.MOV_immediate);
}

// STUR
void handle_stur() {
	mem_write_32(NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address, NEXT_STATE.REGS[HOLDER.Rt]);
	printf("DT_address: %x\n", HOLDER.DT_address);
}

// STURB
void handle_sturb() {
	mem_write_32(NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address, get_memory_segment(0, 7, NEXT_STATE.REGS[HOLDER.Rt])); // get byte only
	printf("WRITE: %x\n", get_memory_segment(0,7, NEXT_STATE.REGS[HOLDER.Rt]));
	printf("WRITE TO: %lx\n", NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address);
	printf("DT_address: %x\n", HOLDER.DT_address);
}

// STURH
void handle_sturh() {
	mem_write_32(NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address, get_memory_segment(0, 15, NEXT_STATE.REGS[HOLDER.Rt])); // get half only
	printf("DT_address: %x\n", HOLDER.DT_address);
}

// STURW
void handle_sturw() {
	mem_write_32(NEXT_STATE.REGS[HOLDER.Rn] + HOLDER.DT_address, get_memory_segment(0, 31, NEXT_STATE.REGS[HOLDER.Rt]));
	printf("DT_address: %x\n", HOLDER.DT_address);
}

// SUB
void handle_sub() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] - NEXT_STATE.REGS[HOLDER.Rm];
}

// SUBI
void handle_subi() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] - HOLDER.ALU_immediate;
}

// SUBS
void handle_subs() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] - NEXT_STATE.REGS[HOLDER.Rm];
	NEXT_STATE.FLAG_N = NEXT_STATE.REGS[HOLDER.Rn] - NEXT_STATE.REGS[HOLDER.Rm] < 0 ? 1 : 0;
	NEXT_STATE.FLAG_Z = NEXT_STATE.REGS[HOLDER.Rn] - NEXT_STATE.REGS[HOLDER.Rm] == 0 ? 1 : 0;
}

// SUBIS
void handle_subis() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] - HOLDER.ALU_immediate;
	NEXT_STATE.FLAG_N = NEXT_STATE.REGS[HOLDER.Rn] - HOLDER.ALU_immediate < 0 ? 1 : 0;
	NEXT_STATE.FLAG_Z = NEXT_STATE.REGS[HOLDER.Rn] - HOLDER.ALU_immediate == 0 ? 1 : 0;
}

// MUL
void handle_mul() {
	NEXT_STATE.REGS[HOLDER.Rd] = NEXT_STATE.REGS[HOLDER.Rn] * NEXT_STATE.REGS[HOLDER.Rm];
}

// HLT
void handle_hlt() {
	RUN_BIT = FALSE;
}

// BR
void handle_br() {
	NEXT_STATE.PC = mem_read_32(HOLDER.Rt);
}

// B
void handle_b() {
	uint64_t offset = sign_extend(HOLDER.BR_address, 26, 2);
	NEXT_STATE.PC = NEXT_STATE.PC + offset;
}

// BEQ & BNE
void handle_beq_bne() {
	uint64_t result = 0;
	if (NEXT_STATE.FLAG_Z == 1) {
		result = 1;	
	}

	if ((HOLDER.Rt & 1) == 1 && (HOLDER.Rt != 15)) {
		result != result;
	}

	if (result) {
		uint64_t offset = sign_extend(HOLDER.COND_BR_address, 19,2);
		NEXT_STATE.PC = NEXT_STATE.PC + offset;
	} else {
		NEXT_STATE.PC += 4;
	}
}

// BGE & BGT
void handle_bge_blt() {
	uint64_t result = 0;
	if (NEXT_STATE.FLAG_N == 0) {
		result = 1;
	}

	if ((HOLDER.Rt & 1) == 1 && (HOLDER.Rt != 15)) {
		result != result;
	}

	if (result) {
		uint64_t offset = sign_extend(HOLDER.COND_BR_address, 19,2);
		NEXT_STATE.PC = NEXT_STATE.PC + offset;
	} else {
		NEXT_STATE.PC += 4;
	}

}

// BGT & BLE
void handle_bgt_ble() {
	uint64_t result = 0;
	if (NEXT_STATE.FLAG_N == 0 && NEXT_STATE.FLAG_Z == 0) {
		result = 1;
	}

	if ((HOLDER.Rt & 1) == 1 && (HOLDER.Rt != 15)) {
		result != result;
	}

	if (result) {
		uint64_t offset = sign_extend(HOLDER.COND_BR_address, 19,2);
		NEXT_STATE.PC = NEXT_STATE.PC + offset;
	} else {
		NEXT_STATE.PC += 4;
	}
}

// END EXECUTE SUBROUTINES
void execute()
{
	// HLT
	if (INSTRUCTION == 0xd4400000) {
		printf("HANDLING HLT\n");
		handle_hlt();
	}

	// ADD
	if (HOLDER.opcode == 0x458 || HOLDER.opcode == 0x459) {
		printf("HANDLING ADD\n");
		handle_add();
	}

	// ADDI
	if (HOLDER.opcode >= 0x488 && HOLDER.opcode <= 0x489) {
		printf("HANDLING ADDI\n");
		handle_addi();
	}

	// ADDS
	if (HOLDER.opcode == 0x558 || HOLDER.opcode == 0x559) {
		printf("HANDLING ADDS\n");
		handle_adds();
	}

	// ADDIS
	if (HOLDER.opcode >= 0x588 && HOLDER.opcode <= 0x589) {
		printf("HANDLING ADDIS\n");
		handle_addis();
	}

	// CBNZ
	if (HOLDER.opcode >= 0x5A8 && HOLDER.opcode <= 0x5AF) {
		printf("HANDLING CBNZ\n");
		handle_cbnz();
	}
	// CBZ
	if (HOLDER.opcode >= 0x5A0 && HOLDER.opcode <= 0x5A7) {
		printf("HANDLING CBZ");
		handle_cbz();
	}
	
	// AND
	if (HOLDER.opcode == 0x450) {
		printf("HANDLING AND\n");
		handle_and();
	}
	
	// ANDS
	if (HOLDER.opcode == 0x750) {
		printf("HANDLING ANDS\n");
		handle_ands();
	}

	// EOR
	if (HOLDER.opcode == 0x650) {
		printf("HANDLING EOR\n");
		handle_eor();
	}

	// ORR
	if (HOLDER.opcode == 0x550) {
		printf("HANDLING ORR\n");
		handle_orr();
	}

	// LDUR - 64 bit 
	if (HOLDER.opcode == 0x7C2) {
		printf("HANDLING LDUR - 64bits\n");
		handle_ldur64();
	}

	// LDURB
	if (HOLDER.opcode == 0x1C2) {
		printf("HANDLING LDURB\n");
		handle_ldurb();
	}

	// LDURH
	if (HOLDER.opcode == 0x3C2) {
		printf("HANDLING LDURH\n");
		handle_ldurh();
	}

	// LSL (Immediate)
	if (HOLDER.opcode == 0x69B) {
		printf("immr: %x\n", get_instruction_segment_sim(16,21));
		printf("imms: %x\n", get_instruction_segment_sim(10,15));
		if (get_instruction_segment_sim(10,15) == 0x3f) {
			handle_lsr();
		} else {
			handle_lsl();
		}
	}

	// LSR (Immediate)
	if (HOLDER.opcode == 0x69A) {
		printf("RIGHT OPCODE\n");
		printf("immr: %x\n", get_instruction_segment_sim(16,21));
		printf("imms: %x\n", get_instruction_segment_sim(10,15));
		if (get_instruction_segment_sim(10,15) != 0x3F) {
			handle_lsl();
		} else {
			handle_lsr();
		}
	}

	// MOVZ
	if (HOLDER.opcode >= 0x694 && HOLDER.opcode <= 0x697) {
		printf("HANDLING MOVZ\n");
		handle_movz();
	}

	// STUR
	if (HOLDER.opcode == 0x7C0) {
		printf("HANDLING STUR\n");
		handle_stur();
	}

	// STURB
	if (HOLDER.opcode == 0x1C0) {
		printf("HANDLING STURB\n");
		handle_sturb();
	}

	// STURH
	if (HOLDER.opcode == 0x3C0) {
		printf("HANDLING STURH\n");
		handle_sturh();
	}

	// STURW
	if (HOLDER.opcode == 0x5C0) {
		printf("HANDLING STURW\n");
		handle_sturw();
	}

	// SUB
	if (HOLDER.opcode == 0x658 || HOLDER.opcode == 0x659) {
		printf("HANDLING SUB\n");
		handle_sub();
	}

	// SUBI
	if (HOLDER.opcode == 0x688 || HOLDER.opcode == 0x689) {
		printf("HANDLING SUBI\n");
		handle_sub();
	}

	// SUBS
	if (HOLDER.opcode == 0x758 || HOLDER.opcode == 0x759) {
		printf("HANDLING SUBS\n");
		handle_subs();
	}

	// SUBIS
	if (HOLDER.opcode == 0x788 || HOLDER.opcode == 0x789) {
		printf("HANDLING SUBIS\n");
		handle_subis();
	}

	// MUL
	if (HOLDER.opcode == 0x4D8) {
		printf("HANDLING MUL\n");
		handle_mul();
	}
	
	// BR
	if (HOLDER.opcode == 0x6B0) {
		printf("HANDLING BR\n");
		handle_br();
	}
	// B
	if (HOLDER.opcode >= 0x0A0 && HOLDER.opcode <= 0x0BF) {
		printf("HANDLING B\n");
		handle_b();
	}

	// BEQ, BNE, BGT, BLT, BGE, BLE 
	if (HOLDER.opcode >= 0x2A0 && HOLDER.opcode <= 0x2A7) {
		uint32_t cond = (HOLDER.Rt & 14) >> 1;
		if (cond == 0) {
			// EQ or NE
			printf("HANDLING BEQ or BNE\n");
			handle_beq_bne();
		} else if (cond == 5) {
			// BGE or BLT
			printf("HANDLING BGE or BLT\n");
			handle_bge_blt();
		} else if (cond == 6) {
			// BGT or BLE
			printf("HANDLING BGT or BLE\n");
			handle_bgt_ble();
		}
	}
}