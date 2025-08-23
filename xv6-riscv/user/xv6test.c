#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/cool_threads.h"

int g = 0;

// volatile int *shared;  // 将指针暴露成全局，便于另一线程读取同一VA

// static void reader(void *arg) {
//   sleep(30);
//   printf("reader sees *shared=%d\n", *shared);
//   for(;;) sleep(1000);
// }

// int main() {
//   // 在线程组里创建 reader
//   ct_create(reader, 0);

//   // 线程0（主线程）分配一页并写入
//   int *p = (int*)sbrk(4096);
//   *p = 12345;
//   shared = p;

//   sleep(200);
//   exit(0);
// }

// void writer(void *arg){
//   int *p = (int*)sbrk(4096);
//   *p = (int)(uint64)arg;
//   sleep(50);
//   printf("writer wrote %d, reader sees %d\n", (int)(uint64)arg, *p);
//   for(;;) sleep(1000);
// }

// int main(){
//   ct_create(writer, (void*)777);
//   sleep(30);
//   ct_create(writer, (void*)888); // 另一个线程也扩一页
//   sleep(300);
//   exit(0);
// }


// volatile int *g_addr = 0;

// static void reader(void *arg) {
//   sleep(50);                      // 等 T1 分配并写入
//   if (!g_addr) {
//     printf("reader: g_addr is NULL\n");
//     exit(1);
//   }
//   printf("reader: *g_addr=%d (va=%p)\n", *g_addr, g_addr);
//   for(;;) sleep(1000);
// }

// int main(void) {
//   // 启动读线程（同一地址空间）
//   ct_create(reader, 0);

//   // T1：用 sbrk 扩一页并写入
//   int *p = (int*)sbrk(4096);
//   if (p == (void*)-1) {
//     printf("writer: sbrk failed\n");
//     exit(1);
//   }
//   *p = 12345;
//   g_addr = p;
//   printf("writer: wrote %d at va=%p\n", *p, p);

//   // 给 reader 留时间读取
//   sleep(200);
//   exit(0);
// }

int tid2 = 0;
static void worker(void *arg){
  printf("worker %d start\n", (int)(uint64)arg);
  // sleep(50);
  // ct_exit();
}

int main(){
  printf("created thread");
  sleep(50);
  int tid1 = ct_create(worker, (void*)42);
  sleep(50);
  int tid2 = ct_join(tid1);
  printf("joined %d\n", tid2);
  exit(0);
}