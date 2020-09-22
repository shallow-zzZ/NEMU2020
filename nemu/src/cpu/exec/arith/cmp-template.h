#include "cpu/exec/template-start.h"

#define instr cmp

#if DATA_BYTE == 4

make_helper(cmp_si2rm_l){
	int len = decode_si2rm_l(eip+1);
	print_asm("%s %s",op_src->str, op_src2->str);
	return len+1;
}

#endif

#if DATA_BYTE == 2

make_helper(cmp_si2rm_w){
        int len = decode_si2rm_w(eip+1);
        print_asm("%s %s",op_src->str, op_src2->str);
        return len+1;
}

#endif
#include "cpu/exec/template-end.h"
