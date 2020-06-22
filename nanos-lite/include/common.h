#ifndef __COMMON_H__
#define __COMMON_H__

#include <am.h>
#include <klib.h>
#include "debug.h"
extern _RegSet* do_syscall(_RegSet *r) ;
extern _RegSet* schedule(_RegSet *prev) ;
typedef char bool;
#define true 1
#define false 0

#endif
