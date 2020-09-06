#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char str[100];
	uint32_t result;
	/* TODO: Add more members if necessary */


} WP;

WP *new_wp();
void free_wp(WP *wp);
void info_wp();
WP *cal_wp(int no);
#endif
