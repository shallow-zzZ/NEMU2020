#include "cpu/exec/template-start.h"

uint32_t pio_write(ioaddr_t, size_t, uint32_t);

make_helper(concat(out_a2i_,SUFFIX)) {
	uint8_t imm = instr_fetch(eip+1, 1);
	pio_write(imm, DATA_BYTE, (DATA_TYPE)reg_l(R_EAX));
	print_asm("out 0x%x", imm);
	return 2;
}

make_helper(concat(out_,SUFFIX)) {
	pio_write(reg_w(R_DX), DATA_BYTE, (DATA_TYPE)reg_l(R_EAX));
	print_asm("out");
	return 1;
}

#include "cpu/exec/template-end.h"
