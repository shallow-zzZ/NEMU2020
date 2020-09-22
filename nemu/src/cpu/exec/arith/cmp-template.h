#include "cpu/exec/template-start.h"

#define instr cmp


make_helper(concat(cmp_i2rm_,SUFFIX)){
	int len = concat(decode_rm_,SUFFIX)(eip+1);
	int8_t src1 = swaddr_read(eip+ len +1,1);
	print_asm("$0x%x %s",src1, op_src->str);
	return len+2;
}

#include "cpu/exec/template-end.h"
