#include "cpu/exec/template-start.h"

#define instr cmp


make_helper(concat(cmp_i2rm_,SUFFIX)){
	int8_t src1 = swaddr_read(eip+1,1);
	print_asm("%d\n", src1);
	//int len = concat(decode_rm_,SUFFIX)(eip+2);
	//print_asm("%x %s",src1, op_src->str);
	return 3;
}

#include "cpu/exec/template-end.h"
