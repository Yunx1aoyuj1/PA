#include "common.h"
#include "fs.h"
#include "memory.h"
#define DEFAULT_ENTRY ((void *)0x8048000)
// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
extern void ramdisk_read(void *buf, off_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
//void ramdisk_write(const void *buf, off_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  int fd = fs_open(filename,0,0);
  size_t size = fs_filesz(fd);
  //fs_read(fd,DEFAULT_ENTRY,size);
  void *va = DEFAULT_ENTRY;
  void *pa = NULL;
  int count_of_page = size/ PGSIZE + 1;
  
  for(int i = 0 ; i < count_of_page ; i++){
    pa = new_page();
    //Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
    _map(as, va, pa);
    fs_read(fd,pa,PGSIZE);
    va += PGSIZE;
  }
  fs_close(fd);
  
  return (uintptr_t)DEFAULT_ENTRY;
}
