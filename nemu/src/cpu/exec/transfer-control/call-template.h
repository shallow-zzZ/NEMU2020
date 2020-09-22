#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	if(DATA_BYTE == 2){
		reg_l(R_ESP) -= 2;
		swaddr_w(reg_l(R_ESP), 2, cpu.eip);
		
	}else{
		reg_l(R_ESP)-=4;
	}
}

make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"
