#include "cpu/exec/template-start.h"

make_helper(concat(iret_,SUFFIX)) {
	cpu.eip = swaddr_read(reg_l(R_ESP), 4, R_SS);
	cpu.sreg[R_CS].val = swaddr_read(reg_l(R_ESP)+4, 4, R_SS);
	LoadScache(R_CS);
	cpu.eflags = swaddr_read(reg_l(R_ESP)+8, 4, R_SS);
	reg_l(R_ESP) += 12;
	print_asm("iret");
	return 2;
}

#include "cpu/exec/template-end.h"
