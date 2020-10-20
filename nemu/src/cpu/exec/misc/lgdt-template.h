#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute() {
	if(op_src->size == 2){
		cpu.gdtr.limit = lnaddr_read(op_src->addr,2); // 2 byte
		cpu.gdtr.base = lnaddr_read(op_src->addr+2,3); // 3 byte, higher 1 byte not use!
	}
	else if(op_src->size == 4){
		cpu.gdtr.limit = lnaddr_read(op_src->addr,2); // 2 byte
		cpu.gdtr.base = lnaddr_read(op_src->addr+2,4); // 4 byte
		printf("%x %x\n", cpu.gdtr.limit, cpu.gdtr.base);
	}
	print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
