#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

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


static int cmd_si(char *args){
  // TODO: 利用 strtok 读取出 N
  char *token = strtok(args," ");
  int step = 0;//initialize
  if(token == NULL){
    step = 1;
  }
  else if(strcmp(token,"-1") == 0)
  {
    cmd_c(NULL);
  }
  else{
  int number,weight,length;//length of token
    length = strlen(token);
    int i;
    for (weight = 1,i = length - 1 ; i >= 0 ; weight *= 10, i --){//change string into integer
      number = token[i] - '0';
      step += number * weight;
    }
  }
  // TODO: 然后根据 N 来执行对应的 cpu_exec(N) 操作
  cpu_exec(step);
  return 0;
}

static int cmd_info(char *args){
  // 分割字符
  char *token = strtok(args," ");

  // 判断子命令是否是r
  if (strcmp(token,"r") == 0){
      // 依次打印所有寄存器
      // using for 
      // 这里给个例子：打印出 eax 寄存器的值
      for(int i = 0; i < 8;i ++)
      //i < 8,because : const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
        printf("%s:\t 0x%08x\t  %d\n", regsl[i], cpu.gpr[i]._32 , cpu.gpr[i]._32);
      
  }
  else if (strcmp(token,"w") == 0){
      // 这里我们会在 PA1.3 中实现
  }
  return 0;
}

static int cmd_x(char *args){
  //分割字符串，得到起始位置和要读取的次数
  char *token = strtok(args," ");
  uint32_t addr = 0;
  int times = 0,weight = 1;
  int i,number;
  //times
  for (weight = 1,i =  strlen(token) - 1;i >= 0 ; weight = weight * 10, i --){//change string into integer
      number = token[i] - '0';
      times += (number * weight);
  }
  //address
  token = strtok(NULL ," ");
  for (weight = 1,i = strlen(token) -1; i > 1; i --,weight *= 16){
    token[i] = toupper(token[i]);
    if(token[i] <='9' && token[i] >= '0'){
      number = token[i] - '0';
      addr += number * weight;
    }
    else{
      number = token[i] - 'A';
      addr += number * weight;
    }  
  }
  
  printf("Address\tDword block\t ... Byte sequence\n");
  //循环使用 vaddr_read 函数来读取内存
  uint32_t data;
  for(i = 0;i < times; i ++){
    data = vaddr_read(addr,4);    //如何调用，怎么传递参数，请阅读代码
    //每次循环将读取到的数据用 printf 打印出来
    printf("0x%08x 0x%08x ...",addr,data);//如果你不知道应该打印什么，可以参考参考输出形式
    for (int j = 0; j < 4; j++){
      int a[2];
      a[0] = data%16;
      data /= 16 ;
      a[1] = data%16;
      data /= 16;
      printf("%X%X ",a[1],a[0]);
    }
    printf("\n");
    addr += 4;
  }
  return 0;
}

static int cmd_p(char *args){
  //分割字符串
  char *token = strtok(args," ");
  bool success = true;
  printf("%s",args);
  int result = expr(token,&success);
  if(success == false)
  {
    printf("expression illegal\n");
  }
  else
  {
    printf("%s:%d\n",token,result);
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

  /* TODO: Add more commands */
  {"si","Single step. si x ,time x", cmd_si},
  {"info","\t -r Print each register information \t -w ",cmd_info},//not finish 
  {"x","\t {times} {address}  Scan memory",cmd_x},
  {"p","\t #{expression}",cmd_p},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
