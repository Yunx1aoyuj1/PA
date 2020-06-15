#include "nemu.h"
#include "device/mmio.h"
#include"memory/mmu.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_id;
  mmio_id = is_mmio(addr);
  if(mmio_id != -1){
     return  mmio_read(addr,len,mmio_id);
  }
  else{
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id;
  mmio_id = is_mmio(addr);
  if(mmio_id != -1){
    mmio_write(addr,len,data,mmio_id);
  }
  else{
    memcpy(guest_to_host(addr), &data, len);
  }

}

paddr_t page_translate(vaddr_t vaddr, bool writting){
  PTE pte;
  PDE pde;

  //Log("vaddr:%#x",vaddr);
  //Log("CR0:%#x",cpu.cr0.val);
  //Log("CR3:%#x",cpu.cr3.val);
  uint32_t Dir = (vaddr  >> 22);
  uint32_t Ped_addr = (cpu.cr3.page_directory_base << 12)+(Dir << 2);
  pde.val = paddr_read(Ped_addr , 4);
  //Log("PED_addr:%#x PED_val:%#x",Ped_addr , pde.val);
  assert(pde.present);

  uint32_t Page = ( (vaddr  >> 12) & 0x3ff );
  uint32_t Pte_addr = (pde.val & 0xfffff000)+(Page << 2);
  pte.val = paddr_read(Pte_addr , 4);
  //Log("PTD_addr:%#x PTD_val:%#x",Pte_addr , pte.val);
  assert(pte.present);

  uint32_t P_addr = (pte.val & 0xfffff000) + (vaddr & 0xfff);
  //Log("Physical_addr:%#x",P_addr);

  pde.accessed = 1;
  paddr_write(Ped_addr, 4, pde.val);

  if(pte.accessed == 0|| ((pte.dirty == 0 ) && writting)){
    pte.accessed = 1;
    pte.dirty = 1;
  }
  paddr_write(Pte_addr, 4, pte.val);

  return P_addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.cr0.paging) {
      if ((((addr << 20) >> 20) + len) > 0x1000) {
          //4k = 0x1000
          /* this is a special case, you can handle it later. */
          assert(0);
      }
      else {
          paddr_t paddr = page_translate(addr,0);
          return paddr_read(paddr, len);
      }
  }
  else
      return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //paddr_write(addr, len, data);
  if(cpu.cr0.paging) {
      if ((((addr << 20) >> 20) + len) > 0x1000) {
          //4k = 0x1000
          /* this is a special case, you can handle it later. */
          assert(0);
      }
      else {
          paddr_t paddr = page_translate(addr,1);
          return paddr_write(paddr, len, data);
      }
  }
  else
      return paddr_write(addr, len, data);
}
