#ifndef COOL_THREADS_H
#define COOL_THREADS_H

#include "kernel/types.h"

// ========== 设计说明 ==========
//
// 这个库封装了线程创建/等待的用户态接口。
// 设计目标：
//  - 隐藏系统调用细节（入口地址、寄存器、栈分配等）
//  - 用户只关心：给函数、给参数、拿到 tid、join
//
// 约定（后续里程碑实现）：
//  - 库内部为新线程分配栈（默认 4 页 = 16KB）
//  - 线程入口原型：void (*start)(void*)
//  - 线程返回用 ct_exit() 或从 start 返回（等价于 exit 线程）
//  - join 按 tid 等待线程结束
//
// ===================================

typedef int tid_t;

enum {
  CT_OK  = 0,
  CT_ERR = -1
};

// 默认栈页数（库内部用；现在只是文档约定）
#define CT_DEFAULT_STACK_PAGES 4

// 创建线程：返回新线程 tid，失败返回负数
tid_t ct_create(ct_start_routine_t start, void *arg);

// 等待线程结束：成功返回 0，失败返回负数
int   ct_join(tid_t tid);


// 结束当前线程（不终止进程）。
void  ct_exit(void);

// 获取当前线程 id。现在仅声明，后续实现。
tid_t ct_get_tid(void);

