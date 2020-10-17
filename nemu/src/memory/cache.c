#include "common.h"


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
}

