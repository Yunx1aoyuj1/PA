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
    printf("no free watch point\n");
    return (NULL);
  }
  else{
    WP* p = free_;
    WP* q = NULL;
    q = head;
    head = p;
    head ->next = q;
    free_ = free_ -> next;
    if(head -> next == 0){
      head -> NO =1;
    }
    else{
      head -> NO = 1 + head -> next -> NO;
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
  for (; (p ->next )-> NO!= wp -> NO && p; p = p -> next);
  if(p){
    r = p -> next ;
    p -> next = r -> next;
    
    q = free_;
    free_ = r;
    free_ -> next =q;

    q = head;
    (p -> NO) --;
    for (; q != p; q = q -> next)
      (q -> NO) --;   
  }
  else{
    printf("this watch point is not using\n");
  }
}

