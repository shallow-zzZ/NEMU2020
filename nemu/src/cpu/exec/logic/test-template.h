#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	DATA_TYPE_S res = op_src->val & op_src2->val;
	cpu.OF = 0;
	cpu.CF = 0;
	cpu.SF = (res<0)? 1:0;
	cpu.ZF = (!res)? 1:0;
	int i = 0, cnt = 0;
	for(i=0;i<8;i++){
		if(res & (1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	print_asm(str(instr) " %s %s", op_src->str, op_src2->str);
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
