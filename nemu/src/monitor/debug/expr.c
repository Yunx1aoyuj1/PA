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

int check_parentheses(int p, int q){
  bool if_surrounded = 0;
  if(tokens[p].type == '(' && tokens[q].type == ')')
    if_surrounded = 1;
  int number_of_bracket = 0 ,i = p;
  /*
    "(" weights 1;
    ")" weights -1;
  */
  for ( ; i <= q; i++){
    if(tokens[i].type == '(' )
      number_of_bracket ++;
    else if( tokens[i].type == ')')
      number_of_bracket --;
  }
  if(number_of_bracket) return 0;//false //an wrong expression
  else if(!number_of_bracket && if_surrounded ) return 1;//true
  else if(!number_of_bracket && !if_surrounded ) return -1;
  return 1;//make it could be compile
}//different return value tell eval what happen when check false.

uint32_t find_dominated_op(int p, int q){
  uint32_t op = 0;
  //当+ （-）号位于两个(,*,/,)之间时，一定是中心操作符。其次如果-前没有操作数一定是负号
  int number_of_bracket = 0;
  for (int i = p ; i <= q; i++){
    if(tokens[i].type == '(' )
      number_of_bracket ++;
    else if( tokens[i].type == ')')
      number_of_bracket --;

    else if (number_of_bracket == 0){
      if(tokens[i].type == '+' || tokens[i].type == '-' || tokens[i].type == '*'  || tokens[i].type == 'c' ){
        if (tokens[i].type == '+' || tokens[i].type == '-' ){
          if(tokens[i].type == '-' &&(i == p || (tokens[i - 1].type != TK_10 || tokens[i - 1].type != TK_16))){//judge if a negative number
              continue;//is a negative number pass.
          }
          else{
            op = i;
          }
          
        }
        else{
          if(tokens[op].type != '+' || tokens[op].type != '-' ){//only * or / be here 
            op = i;
          }
        }
      }
    }
  }
  return op;
}

uint32_t eval(int p,int q) {
    int k = check_parentheses(p, q);
    /*for (int i = 0; i < 32; i++)
    {
      printf("%d,%s\n",tokens[i].type,tokens[i].str);
    }*/
    //printf("%d %d %d \n",k,p,q);

    if(k == 0)
    {
      printf("have wrong with number of brackets\n");
      assert(0);
    }

    if (p > q) {
        return 0;
    }

    else if (p == q) {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
        //transfor string to int 
        int length = strlen(tokens[p].str);
        int weight = tokens[p].type;
        int sum = 0;
        for (int i = length -1; i >= 0; i-- ,weight *=weight)
        {
          if(tokens[p].str[i] >= '0' && tokens[p].str[i] <='9')
            sum +=  (tokens[p].str[i] - '0') * weight;

          else if(tokens[p].str[i] >= 'a' && tokens[p].str[i] <='f')
            sum +=  (tokens[p].str[i] - 'a' + 10) * weight;

          else if(tokens[p].str[i] >= 'A' && tokens[p].str[i] <='F')
            sum +=  (tokens[p].str[i] - 'A' + 10 )  * weight;
        }
        
        return sum;
    }

    else if ( k == true) {
        /* The expression is surrounded by a matched pair of parentheses.
        * If that is the case, just throw away the parentheses.
        */
        return eval(p + 1, q - 1);
    }

    else {
      //int success;
      int op = find_dominated_op( p, q);
      uint32_t val1 = eval(p, op - 1);
      uint32_t val2 = eval(op + 1, q);
      
      switch (tokens[op].type) {
          case '+': return val1 + val2; break;
          case '-': return val1 - val2; break;
          case '*': return val1 * val2; break;
          case '/': return val1 / val2; break;
          default:assert(0);
      }
    }
}


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
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        bool state = false;
        if(rules[i].token_type == TK_NOTYPE) continue;//空格不记录
        //else a new token 
        switch (rules[i].token_type) {
          case '+':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '-':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '*':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '/':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_EQ:{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_10:{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_16:{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case '(':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case ')':{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;

          case TK_REGISTER:{
            tokens[nr_token].type = rules[i].token_type;
            state = 1;
          }break;



          default: return false;
        }

        if(state){
        // now it is a new and right token
          char *point = (char *)calloc(strlen(substr_start),sizeof(char));        // calloc space for token
          if(point == 0)
          {
            printf("\nerror!\n");
            assert(0);
          } 
          free(tokens[nr_token].str);
          tokens[nr_token].str = point;
          strcpy(tokens[nr_token].str,substr_start);
          nr_token ++;
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

  /*for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == certain type) ) {
        tokens[i].type = DEREF;
    }
  }*/
  return eval(0,nr_token - 1);
}

