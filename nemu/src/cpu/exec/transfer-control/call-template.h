#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	if(DATA_BYTE == 2){
		reg_l(R_ESP) -= 2;
		swaddr_write(reg_l(R_ESP), 2, cpu.eip);
		cpu.eip = (cpu.eip+ op_src->val)& 0x0000ffff;
	}else{
		reg_l(R_ESP)-=4;
		swaddr_write(reg_l(R_ESP), 4, cpu.eip);
		cpu.eip = cpu.eip + op_src->val;
	}
	print_asm_template1();
}

make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
