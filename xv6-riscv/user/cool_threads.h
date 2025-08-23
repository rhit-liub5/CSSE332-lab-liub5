#define COOL_THREADS_H

#include "kernel/types.h"
#include "user/user.h"

typedef int tid_t;
typedef void (*ct_start_routine_t)(void *);

#define CT_DEFAULT_STACK_PAGES 4

static void __ct_thread_ret(void) {
  thread_exit();
  for(;;) { } // 防止返回
}

tid_t ct_create(ct_start_routine_t start, void *arg){
    return thread_create(start, arg, __ct_thread_ret);
};

int   ct_join(tid_t tid){
    
    return thread_join(tid);
};


void  ct_exit(void){
    thread_exit();
    for(;;) { }
};

tid_t ct_get_tid(void);
