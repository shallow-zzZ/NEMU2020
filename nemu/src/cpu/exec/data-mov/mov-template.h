#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX), R_DS);

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr, R_DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE == 4
make_helper(mov_cr2r){
	uint8_t opcode = instr_fetch(eip + 1, 1);
	if(opcode == 0xc0) {
		cpu.eax = cpu.cr0.val;
		print_asm("mov" str(SUFFIX) " %%cr0 %%eax");
	}else if(opcode == 0xd8) {
		cpu.eax = cpu.cr3.val;
		print_asm("mov" str(SUFFIX) " %%cr3 %%eax");
	}
	return 2;
}

make_helper(mov_r2cr){
	uint8_t opcode = instr_fetch(eip + 1, 1);
	if(opcode == 0xc0) {
		cpu.cr0.val = cpu.eax;
		print_asm("mov" str(SUFFIX) " %%eax %%cr0");
	}else if(opcode == 0xd8) {
		cpu.cr3.val = cpu.eax;
		print_asm("mov" str(SUFFIX) " %%eax %%cr3");
	}
	return 2;
}
#endif

#if DATA_BYTE == 2
make_helper(mov_rm2sreg){
	uint8_t opcode = instr_fetch(eip+1,1);
	if(opcode == 0xd8) { // DS
		cpu.sreg[R_DS].val = reg_w(R_AX);
		LoadScache(R_DS);
		print_asm("mov sreg %%DS");
	}
	if(opcode == 0xc0) { // ES
		cpu.sreg[R_ES].val = reg_w(R_AX);
		LoadScache(R_ES);
		print_asm("mov sreg %%ES");
	}
	if(opcode == 0xd0) { // SS
		cpu.sreg[R_SS].val = reg_w(R_AX);
		LoadScache(R_SS);
		print_asm("mov sreg %%SS");
	}
	return 2;
}
#endif

#include "cpu/exec/template-end.h"
