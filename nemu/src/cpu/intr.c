#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
  rtl_push(&cpu.eflags.Initial_Value);
  cpu.eflags.IF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  if(NO > cpu.idtr.limit)
    assert(0);
  
  vaddr_t addr_of_gate = cpu.idtr.base + NO * 8;
  uint32_t low,high;
  low = vaddr_read(addr_of_gate,4) & 0x0000ffff;
  high = vaddr_read(addr_of_gate + 4,4) & 0xffff0000;
  decoding.jmp_eip = low | high;
  decoding.is_jmp = 1;
  //printf("get a interrupt %d at address 0x%x\n",NO,ret_addr);
}

void dev_raise_intr() {
  cpu.INTR = true;
}
