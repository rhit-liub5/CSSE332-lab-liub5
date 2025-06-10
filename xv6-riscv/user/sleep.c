#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv)
{
  /* TODO: Insert your code here. */
    if(argc < 2){
        printf("Usage: sleep <ticks>\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);

    sleep(ticks);

    exit(0);
  
}
