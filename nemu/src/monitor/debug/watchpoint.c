#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
/*
  代码中定义了监视点结构的池 wp_pool，还有两个链表 head 和 free_，
  其中 head 用于组织使用中的监视点结构，free_ 用于组织空闲的监视点结构。
*/

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/*  
  其中 new_wp() 从 free_ 链表中返回一个空闲的监视点结构，
  free_wp() 将 wp 归还到 free_ 链表中，
  这两个函数会作为监视点池的接口被其它函数调用。
  需要注意的是，调用 new_wp() 时可能会出现没有空闲监视点结构的情况，
  为了简单起见，此时可以通过 assert(0) 马上终止程序。
  框架代码中定义了 32 个监视点结构，一般情况下应该足够使用，
  如果你需要更多的监视点结构，你可以修改 NR_WP 宏的值。
*/

WP* new_wp(){// changed head and free_
  if(!free_){
    
    return (NULL);
  }
  else{
    WP* p = free_;
    WP* q = NULL;
    if(head == NULL){
      free_ = free_ -> next;
      q = head;
      head = p;
      head ->next = NULL;
    }
    else{
      free_ = free_ -> next;
      q = head;
      for (;q -> next; q = q ->next);
      q -> next = p;
      p -> next = NULL;
    }
    return p;
  }
}

void free_wp(WP *wp){
  if(wp == 0)
  {
    printf("free_wp get an empty WP point \n");
    return ;
  }
  WP *p,*q,*r;
  p =head;
  if(p -> NO == wp -> NO){
    head = p -> next;
    q = free_;
    free_ = p;
    free_ -> next = q;
  }
  else{
    for (; (p ->next )-> NO!= wp -> NO && p; p = p -> next);
    if(p){
      r = p -> next ;
      p -> next = r -> next;
      
      q = free_;
      free_ = r;
      free_ -> next =q;

      q = head;
      (p -> NO) --;
    }
    else{
      printf("this watch point is not using\n");
    }
  }
}

int set_watchpoint(char *e){    //给予一个表达式e，构造以该表达式为监视目标的监视点，并返回编号
  WP *wp = new_wp();
  if(wp == 0){
    printf("no free watch point\n");
    return 0;
  }
  strcpy(wp -> expr , e);
  if(strncmp(wp -> expr , "$eip ==",7) == 0){//break;
    printf("break %s set",wp ->expr);
  }

  bool success =true;
  wp -> old_val = expr(wp -> expr,&success);
  if(success == 0){
    printf("set_watchpoint error");
    return (-1);
  }
  printf("set watchpoint #%d \nexpr = %s\nold value =  0x%08x\n",wp -> NO,e,wp -> old_val);
  return (wp -> NO);
}

bool delete_watchpoint(int NO){ //给予一个监视点编号，从已使用的监视点中归还该监视点到池中
  printf("Watchpoint %d delete\n",NO);
  WP wp ;
  wp.NO = NO;
  free_wp(&wp);
  return true;
}

void list_watchpoint(void){     //显示当前在使用状态中的监视点列表
  WP *wp = head;
  if(!head){
    printf("No watchpoint!\n");
    return ;
  }
  printf("NO Expr\t\tOld Value\n");
  for ( ; wp;  wp = wp -> next){
    printf("%02d %s\t\t0x%08X \n",wp -> NO,wp -> expr ,wp -> old_val);
  }
}


/*
  每当 cpu_exec() 执行完一条指令，
  就对所有待监视的表达式进行求值（你之前已经实现表达式求值的功能了），
  比较它们的值有没有发生变化（即新值和旧值做比较），若发生了变化，程序就因触发了监视点而暂停下来，
  你需要将 nemu_state 变量设置为 NEMU_STOP 来达到暂停的效果，
  并用变化后的值覆盖旧值（即作为新的旧值）
*/
WP* scan_watchpoint(void){      //扫描所有使用中的监视点，返回触发的监视点指针，若无触发返回NULL
  WP *wp = head;
  WP *ret;
  int number = 0;
  if(!head){
    printf("No watchpoint!\n");
    return NULL;
  }

  bool success = true;

  for ( ; wp;  wp = wp -> next){
    wp -> new_val = expr(wp ->expr,&success);
    if(!success)
      printf("Error!fail to eval NO%d expression",wp -> NO);
    else if(wp -> new_val != wp -> old_val){
      if(strncmp(wp -> expr , "$eip ==",7) == 0){//break;
        printf("hint : break %s",wp -> expr);
        return wp;
      }
      else{
        bool success = 1;
        printf("\nwatchpoint NO.%d`s old_val has changed @%08x\n",wp -> NO,expr("$eip",&success));
        //tell where 
        printf("expression = %s\n",wp -> expr);
        printf("\nold_val :%d\nnew_val:%d",wp -> old_val , wp -> new_val);
        printf("program paused\n");
        wp -> old_val = wp -> new_val;
        ret = wp;
      }
    }
  }
  if(number)
    return ret;
  else
    return NULL;
}