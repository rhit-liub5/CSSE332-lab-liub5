#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct cpu cpus[NCPU];

struct proc proc[NPROC];

struct proc *initproc;

int nextpid = 1;
struct spinlock pid_lock;

extern void forkret(void);
static void freeproc(struct proc *p);

extern char trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;
static struct proc* allocproc(void);

uint64 spoon(void *arg)
{
  printf("In spoon system call with argument %p\n", arg);
  return 0;
}

static inline struct proc* group_leader(struct proc *p) {
  return p->is_thread ? p->father : p;
}

// 判断 q 是否属于以 leader 为组长的线程组（包含 leader 自己）。
static inline int in_same_group(struct proc *q, struct proc *leader) {
  if(q->state == UNUSED) return 0;
  return (q == leader) || (q->is_thread && q->father == leader);
}

uint64 thread_create(void (*start_fn)(void *), void *arg)
{
 int i;
  struct proc *np;
  struct proc *p = myproc();

  // 1) 分配新的 schedulable 实体（内核栈、trapframe、空用户页表（含 trampoline+trapframe 映射））
  if((np = allocproc()) == 0){
    return -1;
  }
  struct proc *leader = group_leader(p);
  np->sz = leader->sz;   // 共享当前地址空间“上界”

  // 2) 共享用户地址空间：把父方 [0, p->sz) 的用户页，按相同虚拟地址映射到子线程的页表
  //    这里我们不复制物理页，只是把相同的物理页再次映射到 np->pagetable，
  uint64 psz = PGROUNDUP(p->sz);
  for (uint64 va = 0; va < psz; va += PGSIZE) {
    // 跳过高地址的特殊页
    if (va >= TRAPFRAME) break;

    pte_t *ppte = walk(p->pagetable, va, 0);
    if (ppte == 0) continue;                 // 父表没这级页表
    if ((*ppte & PTE_V) == 0) continue;      // 无效
    if ((*ppte & PTE_U) == 0) continue;      // 不是用户页(比如内核/特殊页)

    uint64 pa   = PTE2PA(*ppte);
    uint flags  = PTE_FLAGS(*ppte) & (PTE_R | PTE_W | PTE_X | PTE_U);

    // 子表已存在？那就不要再 map，避免 remap
    pte_t *cpte = walk(np->pagetable, va, 0);
    if (cpte && (*cpte & PTE_V)) {
      continue; // 正常情况下不会发生，但保守处理
    }

    if (mappages(np->pagetable, va, PGSIZE, pa, flags) != 0) {
      freeproc(np);
      release(&np->lock);
      return -1;
    }
  }
  np->sz = p->sz;

  // 3) 独立的用户栈（仅映射在子线程的页表中）
  //    简化方案：在 np 的地址空间末尾增加 1 页作为栈（里程碑2允许不把这页同步回其它线程）

  // 把这段 [base, top) 的映射广播到组内所有线程（包括父线程本身和新线程np）
  int slot = np->pid;
  uint64 guard = TRAPFRAME - (2ULL + 2ULL * slot) * PGSIZE;
  uint64 base  = guard + PGSIZE;
  uint64 top   = base  + PGSIZE;

  if (uvmalloc(np->pagetable, base, top, PTE_W) == 0) {
    freeproc(np);
    release(&np->lock);
    return -1;
  }

  np->start = base;
  np->size  = PGSIZE;
  uint64 sp = top;

  // 入口寄存器（只做一次）
  *(np->trapframe) = *(p->trapframe);
  np->trapframe->epc = (uint64)start_fn;
  np->trapframe->sp  = sp;
  np->trapframe->a0  = (uint64)arg;

  // 安全检查
  if (walkaddr(np->pagetable, sp - 16) == 0)
    panic("thread_create: stack not mapped");

  // 5) 共享打开文件（与 fork 相同地 filedup），cwd 也共享
  for(i = 0; i < NOFILE; i++){
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  }
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(np->name));

  // 6) 线程组标记与亲缘关系
  np->is_thread = 1;
  np->father    = (p->is_thread ? p->father : p); // 组长（拥有地址空间的那个）
  np->tid       = np->pid;                        // 里程碑2：直接用 pid 当 tid

  int tid = np->tid;

  // 7) 常规 parent / 可运行
  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;   
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  return tid;
}

uint64 thread_join(int tid) {
  // 暂时不实现
  return 0;
}


// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
void
proc_mapstacks(pagetable_t kpgtbl)
{
  struct proc *p;
  
  for(p = proc; p < &proc[NPROC]; p++) {
    char *pa = kalloc();
    if(pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((int) (p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
void
procinit(void)
{
  struct proc *p;
  
  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  for(p = proc; p < &proc[NPROC]; p++) {
      initlock(&p->lock, "proc");
      p->state = UNUSED;
      p->kstack = KSTACK((int) (p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu*
mycpu(void)
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

// Return the current struct proc *, or zero if none.
struct proc*
myproc(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}

int
allocpid()
{
  int pid;
  
  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  return pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  //线程部分初始化
  p->is_thread = 0;   
  p->father    = 0;
  p->tid       = 0;   
  p->start     = 0;
  p->size      = 0;
  return p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  p->trapframe = 0;
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;

  //线程部分清理
  p->is_thread = 0;
  p->father    = 0;
  p->tid       = 0;
  p->start     = 0;
  p->size      = 0;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  if(pagetable == 0)
    return 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  if(mappages(pagetable, TRAMPOLINE, PGSIZE,
              (uint64)trampoline, PTE_R | PTE_X) < 0){
    uvmfree(pagetable, 0);
    return 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  if(mappages(pagetable, TRAPFRAME, PGSIZE,
              (uint64)(p->trapframe), PTE_R | PTE_W) < 0){
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// a user program that calls exec("/init")
// assembled from ../user/initcode.S
// od -t xC ../user/initcode
uchar initcode[] = {
  0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x45, 0x02,
  0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x35, 0x02,
  0x93, 0x08, 0x70, 0x00, 0x73, 0x00, 0x00, 0x00,
  0x93, 0x08, 0x20, 0x00, 0x73, 0x00, 0x00, 0x00,
  0xef, 0xf0, 0x9f, 0xff, 0x2f, 0x69, 0x6e, 0x69,
  0x74, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

// Set up first user process.
void
userinit(void)
{
  struct proc *p;

  p = allocproc();
  initproc = p;
  
  // allocate one user page and copy initcode's instructions
  // and data into it.
  uvmfirst(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;

  // prepare for the very first "return" from kernel to user.
  p->trapframe->epc = 0;      // user program counter
  p->trapframe->sp = PGSIZE;  // user stack pointer

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n) {
  struct proc *p = myproc();
  struct proc *leader = p->is_thread ? p->father : p;

  uint64 oldsz = leader->sz;
  uint64 newsz = oldsz;

  if (n > 0) {
    newsz = uvmalloc(leader->pagetable, oldsz, oldsz + n, PTE_W);
    if (newsz == 0) return -1;

    // 广播新映射（跳过已存在映射，避免 remap）
    for (uint64 va = PGROUNDUP(oldsz); va < newsz; va += PGSIZE) {
      if (va >= TRAPFRAME) break; // 绝不碰特殊页
      uint64 pa = walkaddr(leader->pagetable, va);

      for (struct proc *q = proc; q < &proc[NPROC]; q++) {
        // 同组，且有页表
        if (!(q && (q == leader || (q->is_thread && q->father == leader)))) continue;
        if (q->pagetable == 0) continue;
        if (q->pagetable == leader->pagetable) continue; // 组长本身已映射

        pte_t *qpte = walk(q->pagetable, va, 0);
        if (qpte && (*qpte & PTE_V)) {
          // 已经有映射就跳过（防止 mappages: remap）
          continue;
        }
        // 与 uvmalloc 一致的用户权限；是否加 X 依你的策略，通常数据页不加 X
        if (mappages(q->pagetable, va, PGSIZE, pa, PTE_U | PTE_R | PTE_W) != 0) {
          // 保守做法：这里不要 panic；简单忽略或记录错误都行
        }
      }
    }

    leader->sz = newsz;
    for (struct proc *q = proc; q < &proc[NPROC]; q++)
      if (q && (q == leader || (q->is_thread && q->father == leader)))
        q->sz = leader->sz;

  } else if (n < 0) {
    newsz = uvmdealloc(leader->pagetable, oldsz, oldsz + n);

    for (uint64 va = PGROUNDUP(newsz); va < PGROUNDUP(oldsz); va += PGSIZE) {
      if (va >= TRAPFRAME) break;
      for (struct proc *q = proc; q < &proc[NPROC]; q++) {
        if (!(q && (q == leader || (q->is_thread && q->father == leader)))) continue;
        if (q->pagetable == 0) continue;
        if (q->pagetable == leader->pagetable) continue;
        // 只撤销映射，不释放物理页（组长已在 uvmdealloc 里处理）
        uvmunmap(q->pagetable, va, 1, 0);
      }
    }

    leader->sz = newsz;
    for (struct proc *q = proc; q < &proc[NPROC]; q++)
      if (q && (q == leader || (q->is_thread && q->father == leader)))
        q->sz = leader->sz;
  }

  return 0;
}


// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *p = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy user memory from parent to child.
  if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
    freeproc(np);
    release(&np->lock);
    return -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  return pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void
reparent(struct proc *p)
{
  struct proc *pp;

  for(pp = proc; pp < &proc[NPROC]; pp++){
    if(pp->parent == p){
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
void
exit(int status)
{
  struct proc *p = myproc();

  if(p == initproc)
    panic("init exiting");

  // Close all open files.
  for(int fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd]){
      struct file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  end_op();
  p->cwd = 0;

  acquire(&wait_lock);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->parent);
  
  acquire(&p->lock);

  p->xstate = status;
  p->state = ZOMBIE;

  release(&wait_lock);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(uint64 addr)
{
  struct proc *pp;
  int havekids, pid;
  struct proc *p = myproc();

  acquire(&wait_lock);

  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(pp = proc; pp < &proc[NPROC]; pp++){
      if(pp->parent == p){
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        if(pp->state == ZOMBIE){
          // Found one.
          pid = pp->pid;
          if(addr != 0 && copyout(p->pagetable, addr, (char *)&pp->xstate,
                                  sizeof(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            return -1;
          }
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          return pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || killed(p)){
      release(&wait_lock);
      return -1;
    }
    
    // Wait for a child to exit.
    sleep(p, &wait_lock);  //DOC: wait-sleep
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  
  c->proc = 0;
  for(;;){
    // Avoid deadlock by ensuring that devices can interrupt.
    intr_on();

    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
      }
      release(&p->lock);
    }
  }
}

// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&p->lock))
    panic("sched p->lock");
  if(mycpu()->noff != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  static int first = 1;

  // Still holding p->lock from scheduler.
  release(&myproc()->lock);

  if (first) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    first = 0;
    fsinit(ROOTDEV);
  }

  usertrapret();
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks p->lock),
  // so it's okay to release lk.

  acquire(&p->lock);  //DOC: sleeplock1
  release(lk);

  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  release(&p->lock);
  acquire(lk);
}

// Wake up all processes sleeping on chan.
// Must be called without any p->lock.
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    if(p != myproc()){
      acquire(&p->lock);
      if(p->state == SLEEPING && p->chan == chan) {
        p->state = RUNNABLE;
      }
      release(&p->lock);
    }
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
int
kill(int pid)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      p->killed = 1;
      if(p->state == SLEEPING){
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  return -1;
}

void
setkilled(struct proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

int
killed(struct proc *p)
{
  int k;
  
  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  return k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int
either_copyout(int user_dst, uint64 dst, void *src, uint64 len)
{
  struct proc *p = myproc();
  if(user_dst){
    return copyout(p->pagetable, dst, src, len);
  } else {
    memmove((char *)dst, src, len);
    return 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int
either_copyin(void *dst, int user_src, uint64 src, uint64 len)
{
  struct proc *p = myproc();
  if(user_src){
    return copyin(p->pagetable, dst, src, len);
  } else {
    memmove(dst, (char*)src, len);
    return 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [USED]      "used",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  struct proc *p;
  char *state;

  printf("\n");
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    printf("%d %s %s", p->pid, state, p->name);
    printf("\n");
  }
}