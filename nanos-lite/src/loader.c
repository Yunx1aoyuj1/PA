#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
extern void ramdisk_read(void *buf, off_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
//void ramdisk_write(const void *buf, off_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
extern size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
