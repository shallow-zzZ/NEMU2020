#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,
	OR, AND, EQ, NEQ, 
	PLUS, MINUS, MUL, DIV,
	NOT, STAR, NEG,
	LEFT, RIGHT, 
	HEX, DEC, REG,
	VAR
	// HEX > DEC 
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", PLUS},					// plus
	{"-", MINUS},                                   // minus
	{"\\*", MUL},                                   // mul
	{"/", DIV},                                     // div
	{"==", EQ},					// equal
	{"!=", NEQ},                                    // not equal
	{"\\&\\&", AND},                                // and
	{"\\|\\|", OR},                             	// or
	{"\\(", LEFT},                                  // left
	{"\\)", RIGHT},                                 // right
	{"!", NOT},                                     // not
	{"0[xX][0-9a-fA-F]+", HEX},                     // hex
	{"[0-9]+", DEC},                                // dec
	{"\\$[a-zA-Z]{2,3}", REG},                      // register
	{"[a-zA-Z][A-Za-z0-9_]*", VAR}			// var
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case(PLUS): tokens[nr_token].type = PLUS;
					nr_token++;
					break;
					case(MINUS): // minus neg
					if(nr_token==0 || tokens[nr_token-1].type <= LEFT) {
						tokens[nr_token].type = NEG; 
                                        } else {
						tokens[nr_token].type = MINUS;
					}
					nr_token++;
                                        break;
					case(MUL): // mul star
					if(nr_token==0 || tokens[nr_token-1].type <= LEFT){
						tokens[nr_token].type = STAR; 
                                        } else {
						tokens[nr_token].type = MUL;
					}
					nr_token++;
                                        break;
                                        case(DIV): tokens[nr_token].type = DIV;
                                        nr_token++;
                                        break;
                                        case(EQ): tokens[nr_token].type = EQ;
                                        nr_token++;
                                        break;
                                        case(NEQ): tokens[nr_token].type = NEQ;
                                        nr_token++;
                                        break;
                                        case(AND): tokens[nr_token].type = AND;
                                        nr_token++;
                                        break;
                                        case(OR): tokens[nr_token].type = OR;
                                        nr_token++;
                                        break;
                                        case(NOT): tokens[nr_token].type = NOT;
                                        nr_token++;
                                        break;
                                        case(HEX): tokens[nr_token].type = HEX;
					if(substr_len>=32){
						strncpy(tokens[nr_token].str, substr_start, 32);
						tokens[nr_token].str[31] = '\0';
					}else {
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
					}
                                        nr_token++;
                                        break;
                                        case(DEC): tokens[nr_token].type = DEC;
					if(substr_len>=32){
						strncpy(tokens[nr_token].str, substr_start, 32);
						tokens[nr_token].str[31] = '\0';
					}else {
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
					}
                                        nr_token++;
                                        break;
                                        case(REG): tokens[nr_token].type = REG;
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
                                        nr_token++;
                                        break;
					case(VAR): tokens[nr_token].type = VAR;
					if(substr_len>=32){
						strncpy(tokens[nr_token].str, substr_start, 32);
						tokens[nr_token].str[31] = '\0';
					}else {
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
					}
                                        nr_token++;
                                        break;
                                        case(LEFT): tokens[nr_token].type = LEFT;
                                        nr_token++;
                                        break;
                                        case(RIGHT): tokens[nr_token].type = RIGHT;
                                        nr_token++;
                                        break;
                                        case(NOTYPE):
                                        break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	assert(nr_token);
	return true; 
}

uint32_t find_dominant(int p, int q) {
	int i,id=-1,flag=0,minn=LEFT;
	for(i=q;i>=p;i--){
		if(flag>0){
			if(tokens[i].type == LEFT) flag--;
			if(tokens[i].type == RIGHT) flag++;
		}else{
			if(tokens[i].type <= minn){
				if(tokens[i].type >= NOT && tokens[i].type <= NEG){
					minn = NEG;
				} else if(tokens[i].type == EQ || tokens[i].type == NEQ) {
					minn = AND;
				} else {
					minn = tokens[i].type;
				}
				id = i;
			} else if(tokens[i].type == RIGHT){
				flag ++;
			}
		}
	}
	return id;
}

bool check_parentheses(int p, int q) {
	if(tokens[p].type == LEFT && tokens[q].type == RIGHT){
		int i,flag=1;
		for(i=p+1;i<q;i++){
			if(tokens[i].type == LEFT) flag++;
			if(tokens[i].type == RIGHT) flag--;
			if(!flag) return false;
		}
		return (flag==1);
	}
	return false;
}

uint32_t eval(int p, int q){
	if(p>q){  // bad expression
		return 0;
	}else if(p==q){
		int n=0;
		printf("%s\n",tokens[p].str);
		if(tokens[p].type == HEX) {
			sscanf(tokens[p].str,"%x",&n);
		} else if (tokens[p].type == DEC) {
			sscanf(tokens[p].str,"%d",&n);
		} else if (tokens[p].type == REG) {
			int i;
			for(i=0;i<8;i++){
				if(!strcasecmp(tokens[p].str+1,regsl[i])) n = reg_l(i);
				if(!strcasecmp(tokens[p].str+1,regsw[i])) n = reg_w(i);
				if(!strcasecmp(tokens[p].str+1,regsb[i])) n = reg_b(i);
			}
			if(!strcasecmp(tokens[p].str,"$eip")){
				n = cpu.eip;
			}
		} else if (tokens[p].type == VAR) {
			printf("wait\n");
			n = 0;
		} else {
			printf("%d\n",tokens[p].type);
			assert(0);
		}
		return n;
	}else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}else{
		int op = find_dominant(p,q);
		Assert(op!=-1,"Invalid expression!\n");
		int val1 = (int)(eval(p,op-1));
		int val2 = (int)(eval(op+1,q));
		switch(tokens[op].type){
			case(PLUS): return val1+val2;
			case(MINUS): return val1-val2;
			case(MUL): return val1*val2;
			case(DIV): return val1/val2;
			case(EQ): return (val1==val2);
			case(NEQ): return (val1!=val2);
			case(AND): return (val1&&val2);
			case(OR): return (val1||val2);
			case(NEG): return (-val2);
			case(NOT): return (!val2);
			case(STAR): return swaddr_read(val2,4);
			default: assert(0);
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	return eval(0,nr_token-1);
}

