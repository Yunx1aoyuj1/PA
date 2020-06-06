#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  TODO();
  return 1;
}

static inline uintptr_t sys_write(_RegSet *r,uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  if(fd == 1 || fd == 2){
    _putc(buf);
    SYSCALL_ARG1(r) = 1;
  }
  else{
    SYSCALL_ARG1(r) = fs_write(fd,(void *)buf,len);
  }
  //fs_write(fd,(void *)buf,len);
  return 1;
}

static inline uintptr_t sys_read(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  TODO();
  return 1;
}

static inline uintptr_t sys_lseek(uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  return fs_lseek(fd, offset, whence);
}

static inline uintptr_t sys_close(uintptr_t fd) {
  TODO();
  return 1;
}

static inline uintptr_t sys_brk(uintptr_t new_brk) {
  TODO();
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
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
