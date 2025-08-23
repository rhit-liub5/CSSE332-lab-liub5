#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/cool_threads.h"

int g = 0;

//test1
// volatile int *shared;

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

//test2: share memory in writer

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

//test3: independent stack test
// void worker(void *arg) {
//   int marker = (int)(uint64)arg;
//   int local = marker;       
//   printf("tid=%d sees local=%d (expected %d)\n", getpid(), local, marker);
//   ct_exit();
// }

// int main() {
//   int t1 = ct_create(worker, (void*)1111);
//   sleep(30);
//   int t2 = ct_create(worker, (void*)2222);

//   ct_join(t1);
//   ct_join(t2);

//   printf("independent stack test done\n");
//   exit(0);
// }
// expected output:
// tid=... sees local=1111 (expected 1111)
// tid=... sees local=2222 (expected 2222)
// independent stack test done

//test4: create and join threads
// static void worker(void *arg){
//   printf("worker %d start\n", (int)(uint64)arg);
// }

// int main(){
//   printf("created thread");
//   sleep(50);
//   int tid1 = ct_create(worker, (void*)42);
//   sleep(50);
//   int tid2 = ct_join(tid1);
//   printf("joined %d\n", tid2);
//   exit(0);
// }
// expected output:
// created threadworker 42 start
// joined 4