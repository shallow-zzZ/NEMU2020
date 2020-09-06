#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

static int nr_wp;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	nr_wp = 0;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP *wp = free_;
	assert(wp != NULL);
	free_ = free_->next;
	wp->next = NULL;
	wp->NO = nr_wp++;
	if(head == NULL) {
		head = wp;
		wp->NO = nr_wp++;
	}else {
		WP *pos = head;
		while(pos->next != NULL) {
			pos = pos->next;
		}
		pos->next = wp;
	}
	return wp;
}

void free_wp(WP *wp) {
	assert(wp!=NULL);
	WP *pos = head;
	if(pos == wp) {
		head = head->next;
		pos = pos->next;
		wp->next = free_;
		free_ = wp;
	}else {
		while(pos->next != NULL) {
			if(pos->next == wp){
				pos->next = wp->next;
				pos = pos->next;
				wp->next = free_;
				free_ = wp;
				break;
			}
			pos = pos->next;
		}
	}
	while(pos != NULL) {
		pos->NO --;
		pos = pos->next;
	}
	nr_wp--;
	return ;
}

void info_wp() {
	WP* wp = head;
	while(wp != NULL){
		printf("WatchPoint%d : expression: %s       origin_result: %x\n",wp->NO,wp->str,wp->result);
		wp = wp->next;
	}
}

WP *cal_wp(int no) {
	WP* wp = head;
	while(wp!=NULL && wp->NO<no){
		wp = wp->next;
	}
	return wp;
}

bool check_wp() {
	WP *wp = head;
	bool keep = true;
	while(wp!=NULL){
		bool flag;
		uint32_t result_now = expr(wp->str,&flag);
		if(result_now != wp->result){
			printf("WatchPoint%d changed! result_1: 0x%x   result_2: 0x%x\n", wp->NO, wp->result, result_now);
			keep = false;
		}
		wp = wp->next;
	}
	return keep;
}
