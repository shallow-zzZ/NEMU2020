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

uint32_t eval(int p, int q){
	return 0;
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

