#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ = 255,TK_10 = 10,TK_16 = 16 ,
  TK_REGISTER = 254,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  /*
    你需要编写上面的定义中所涉及的最简单的规则，即：
    十进制数字、十六进制数字，如 0x1234，567；
    现阶段所定义的 9 个寄存器，如 $eax, $ebx；
    左括号、右括号；
    加号、减号、乘号、除号；
    空格串（一个或多个空格）。
    你应该把这些规则添加到规则数组中。
  */
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         //减号
  {"\\*", '*'},         //乘号
  {"\\/", '/'},         //除号
  
  {"==", TK_EQ},        // equal

  {"[0-9]+",TK_10},             //十进制数字
  {"0x[0-9a-fA-F]+",TK_16},   //十六进制数字
  

  {"\\(",'('},          //左括号
  {"\\)",')'},          //右括号 

  {"\\$[a-ehilpx]{2,3}", TK_REGISTER},//寄存器

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

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char *str;//using char point
} Token;

Token tokens[32];

int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        printf("%s", substr_start);
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        bool state = false;
        if(rules[i].token_type == TK_NOTYPE) continue;//空格不记录
        //else a new token 
        switch (rules[i].token_type) {
          case '+':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '-':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '*':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '/':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_EQ:{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_10:{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_16:{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '(':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case ')':{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_REGISTER:{
            nr_token ++;
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;



          default: return false;
        }

        if(state){
        // now it is a new and right token
          char *point = calloc(1,sizeof(substr_start));        // calloc space for token
          if(point == 0)
          {
            printf("\nerror!\n");
            exit(0);
          } 
          free(tokens[nr_token].str);
          tokens[nr_token].str = point;
          strcpy(tokens[nr_token].str,substr_start);
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
}
