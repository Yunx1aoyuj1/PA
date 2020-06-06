#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(_RegSet *r, uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  //TODO();
  SYSCALL_ARG1(r) = fs_open((char*)pathname,flags,mode);
  return 1;
}

static inline uintptr_t sys_write(_RegSet *r,uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  SYSCALL_ARG1(r) = fs_write(fd,(void *)buf,len);
  return 1;
}

static inline uintptr_t sys_read(_RegSet *r, uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  SYSCALL_ARG1(r) = fs_read(fd,(void *)buf,len);
  return 1;
}

static inline uintptr_t sys_lseek(_RegSet *r, uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  SYSCALL_ARG1(r) = fs_lseek(fd, offset, whence);
  return 1;
}

static inline uintptr_t sys_close(_RegSet *r,uintptr_t fd) {
  //TODO();
  SYSCALL_ARG1(r) = fs_close(fd);
  return 1;
}

static inline uintptr_t sys_brk(_RegSet *r) {
  //TODO();
  SYSCALL_ARG1(r) = 0;
  return 1;
}

static inline uintptr_t sys_none(_RegSet *r) {
  SYSCALL_ARG1(r) = 1;
  return 1;
}

static inline uintptr_t sys_exit(_RegSet *r) {
  int t0 = SYSCALL_ARG2(r);
  _halt(t0);
  return 1;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none:
      sys_none(r);
      break;
    case SYS_exit:
      sys_exit(r);
      break;
    case SYS_write:
      sys_write(r,a[1],a[2],a[3]);
      break;
    case SYS_brk:
      sys_brk(r);
      break;
    case SYS_open:
      sys_open(r,a[1],a[2],a[3]);
      break;
    case SYS_read:
      sys_read(r,a[1],a[2],a[3]);
      break;
    case SYS_close:
      sys_close(r,a[1]);
      break;
    case SYS_lseek:
      sys_lseek(r,a[1],a[2],a[3]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
