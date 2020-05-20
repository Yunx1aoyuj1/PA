#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  TODO();
  //printf("get a interrupt %d at address 0x%x\n",,ret_addr);
}

void dev_raise_intr() {
}
