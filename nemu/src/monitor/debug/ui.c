#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
	if(args == NULL) {
		cpu_exec(1);
		return 0;
	}
	int n;
	sscanf(args, "%d", &n);
	cpu_exec(n);
	return 0;
}

static int cmd_i(char *args) {
	if(args == NULL) {
		printf("Need More Arguements!\n");
		return 0;
	}
	if(!strcmp(args,"r")) {
		printf("$eax: 0x%x\n", cpu.eax);
		printf("$edx: 0x%x\n", cpu.edx);
		printf("$ecx: 0x%x\n", cpu.ecx);
		printf("$ebx: 0x%x\n", cpu.ebx);
		printf("$ebp: 0x%x\n", cpu.ebp);
		printf("$esi: 0x%x\n", cpu.esi);
		printf("$edi: 0x%x\n", cpu.edi);
		printf("$esp: 0x%x\n", cpu.esp);
		printf("$eip: 0x%x\n", cpu.eip);
	} else{
		info_wp();
	}
	return 0;	
}

static int cmd_p(char *args) {
	bool flag = true;
	uint32_t result = expr(args, &flag);
	if(!flag){
		printf("Cannot recognise the expression!\n");
		return 0;
	}
	printf("epression: %s  result: 0x%x\n",args,result);
	return 0;
}

static int cmd_x(char *args) {
	if(args == NULL) {
		printf("Need More Arguements!\n");
		return 0;
	}
	int n,addr,i;
	char num[50];
	sscanf(args, "%s",num);
	sscanf(num, "%d", &n);
	args += strlen(num)+1;
	assert(args!=NULL);
	bool flag = true;
	addr = expr(args, &flag);
	if(!flag){
		printf("Cannot recognise the expression!\n");
		return 0;
	}
	for(i=0;i<n;i++) {
		printf("addr 0x%x: 0x%x\n", addr, swaddr_read(addr,4,R_DS));
		addr+=4;
	}
	return 0;
}

static int cmd_w(char *args) {
	if(args == NULL) {
		printf("Need More Arguement!\n");
		return 0;
	}
	WP* wp ;
	wp = new_wp();
	strncpy(wp->str,args,strlen(args)+1);
	bool flag = true;
	wp->result = expr(args,&flag);
	if(!flag){
		printf("Cannot recognise the expression!\n");
		free_wp(wp);
		return 0;
	}
	//printf("0x%x\n",wp->result);
	return 0;
}

static int cmd_d(char *args) {
	if(args == NULL) {
                printf("Need More Arguement!\n");
                return 0;
        }
	int no;
	sscanf(args, "%d", &no);
	WP *wp = cal_wp(no);
	free_wp(wp);
	return 0;
}

void getFunName(swaddr_t addr, char *s);

static int cmd_bt(char *args) {
	swaddr_t now_ebp = reg_l(R_EBP);
	swaddr_t now_ret = cpu.eip;
	int cnt = 0, i;
	char name[32];
	while(now_ebp) {
		getFunName(now_ret, name);
		if(name[0] == '\0') break;
		printf("%d 0x%x: ", ++cnt, now_ret);
		printf("%s ", name);
		for(i = 0; i < 4; i++) {
			printf("%d", swaddr_read(now_ebp + 8 + i * 4, 4, R_DS));
			if(i==3) printf("\n");
			else printf(", ");
		}
		now_ret = swaddr_read(now_ebp + 4, 4, R_DS);
		now_ebp = swaddr_read(now_ebp, 4, R_DS);
		printf("\n");
	}
	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Execute by steps", cmd_si},
	{ "info", "Check status of the program", cmd_i},
	{ "p", "calculate the expression", cmd_p},
	{ "x", "Scan the memory", cmd_x}, 
	{ "w", "Set the watchpoint", cmd_w},
	{ "d", "Delete the watchpoint", cmd_d}, 
	{ "bt", "Print Stack", cmd_bt},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
