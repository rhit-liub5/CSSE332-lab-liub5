#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/cool_threads.h"

int g = 0;


// static void worker(void *arg) {
//   int me = (int)(uint64)arg;
//   g += me;
//   printf("I am %d current %d\n", me, g);
//   for(;;)
//   sleep(1000); 
// }

// int main() {
//   ct_create(worker, (void*)10);
//   sleep(50);
//   ct_create(worker, (void*)20);
//   sleep(1000);
//   exit(0);
// }


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

void writer(void *arg){
  int *p = (int*)sbrk(4096);
  *p = (int)(uint64)arg;
  sleep(50);
  printf("writer wrote %d, reader sees %d\n", (int)(uint64)arg, *p);
  for(;;) sleep(1000);
}

int main(){
  ct_create(writer, (void*)777);
  sleep(30);
  ct_create(writer, (void*)888); // 另一个线程也扩一页
  sleep(300);
  exit(0);
}