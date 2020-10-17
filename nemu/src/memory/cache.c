#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define GRP_WIDTH 7
#define OFF_WIDTH 6
#define TAG_WIDTH (27 - GRP_WIDTH - OFF_WIDTH)
#define VALID_WIDTH 1

typedef struct{
	uint32_t valid : VALID_WIDTH;
	uint32_t tag   : TAG_WIDTH;
	uint8_t block[64]; 
}Cache1;

typedef union {
	struct {	
		uint32_t offset : OFF_WIDTH;
		uint32_t grp : GRP_WIDTH;
		uint32_t tag : TAG_WIDTH;
	};
	uint32_t addr;
} cache_addr;

#define NR_GRP (1 << GRP_WIDTH)
#define NR_LINE 8

Cache1 L1[NR_GRP][NR_LINE];

void init_l1() {
	int i, j;
	for(i=0;i<NR_GRP;i++){
		for(j=0;j<NR_LINE;j++){
			L1[i][j].valid = 0;
		}
	}
	printf("init cache!\n");
}

uint32_t L1_read(hwaddr_t addr, size_t len) {
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}

void L1_write(hwaddr_t addr, size_t len, uint32_t data) {
	printf("write in cache!\n");
	dram_write(addr, len, data);
}
