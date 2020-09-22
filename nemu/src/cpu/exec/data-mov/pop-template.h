#include "cpu/exec/template-start.h"

#define instr pop

make_helper(concat(pop_r_,SUFFIX)){
	int len = concat(decode_r_,SUFFIX)(eip+1);
	print_asm_template1();
	return len+1;
}

#include "cpu/exec/template-end.h"
