#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;
	OPERAND_W(op_src, result);

	/* TODO: Update EFLAGS. */
	//panic("please implement me");
	int len = DATA_BYTE << 3;
	cpu.OF = (op_src->val == ((1<<(len-1))-1))? 1:0;
	cpu.SF = (result >> (len-1));
	cpu.ZF = (!result)? 1:0;
	int i = 0, cnt = 0;
	for(i=0;i<8;i++){
		if(result & (1<<i)) cnt++;
	}
	cpu.PF = (cnt%2)? 0:1;
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
