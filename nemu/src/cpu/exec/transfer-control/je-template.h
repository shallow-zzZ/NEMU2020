#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	if(cpu.ZF) cpu.eip += op_src->val;
	if(DATA_BYTE == 2) cpu.eip = cpu.eip & 0x0000ffff;
	print_asm(str(instr) " %s", op_src->str);
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
