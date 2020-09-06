#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,
	PLUS, MINUS, MUL, DIV, 
	EQ, NEQ, AND, OR,
	NOT, STAR, NEG,
	LEFT, RIGHT, 
	HEX, DEC, REG

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
	{"\\$[a-zA-Z]{2,3}", REG}                       // register
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

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
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
					strncpy(tokens[nr_token].str, substr_start, ((substr_len>31)?31:substr_len) + 1);
                                        nr_token++;
                                        break;
                                        case(DEC): tokens[nr_token].type = DEC;
					strncpy(tokens[nr_token].str, substr_start, ((substr_len>31)?31:substr_len) + 1);
                                        nr_token++;
                                        break;
                                        case(REG): tokens[nr_token].type = REG;
					strncpy(tokens[nr_token].str, substr_start, substr_len + 1);
                                        nr_token++;
                                        break;
                                        case(LEFT): tokens[nr_token].type = LEFT;
                                        nr_token++;
                                        break;
                                        case(RIGHT): tokens[nr_token].type = RIGHT;
                                        nr_token++;
                                        break;
                                        case(NOTYPE): tokens[nr_token].type = NOTYPE;
                                        nr_token++;
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

	return true; 
}

uint32_t find_dominant(int p, int q) {
	int i,id=-1,flag=0,minn=OR;
	for(i=q;i>=p;i--){
		if(flag>0){
			if(tokens[i].type == LEFT) flag--;
			if(tokens[i].type == RIGHT) flag++;
		}else{
			if(tokens[i].type <= minn){
				minn = tokens[i].type;
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
		switch(tokens[p].type){
			case(HEX): return 1;
			case(DEC): return 1;
			case(REG): return 1;
			default: assert(0);
		}
	}else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}else{
		int op = find_dominant(p,q);
		assert(op!=-1);
		uint32_t val1 = eval(p,op-1);
		uint32_t val2 = eval(op+1,q);
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
	int i;
	for(i=0;i<nr_token;i++){
		printf("%d\n",tokens[i].type);
	}
	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return eval(0,nr_token-1);
}

