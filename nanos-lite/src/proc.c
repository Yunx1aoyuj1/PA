#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
static uint32_t schedule_count = 0;
static uint32_t schedule_change_count = 400;

PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  current -> tf = prev;
  schedule_count ++;
  //current = &pcb[0];
  if((schedule_count >= schedule_change_count) && (current != &pcb[1])){
    current = &pcb[1];
    schedule_count = 0;
  }
  else{
    
    current = &pcb[0];
  }
  _switch(&(current -> as));
  return current -> tf;
}
