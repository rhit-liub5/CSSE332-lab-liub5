#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

pthread_cond_t  c;
pthread_mutex_t m;
int number = 0;



void *thread(void *arg)
{
	char *letter = (char *)arg;
	pthread_mutex_lock(&m);
	number++;
	printf("%c wants to enter the critical section\n", *letter);
	while(number > 3){
		printf("%c is waiting to enter the critical section\n", *letter);
		pthread_cond_wait(&c, &m);
	}


	printf("%c is in the critical section\n", *letter);
	sleep(1);
	number--;
	// pthread_cond_signal(&c);
	pthread_cond_broadcast(&c);
	pthread_mutex_unlock(&m);	
	printf("%c has left the critical section\n", *letter);

	return NULL;
}

int
main(int argc, char **argv)
{
	pthread_t threads[8];
	int i;
	char *letters = "abcdefgh";

	for(i = 0; i < 8; ++i) {
		pthread_create(&threads[i], NULL, thread, &letters[i]);

		if(i == 4)
			sleep(4);
	}

	for(i = 0; i < 8; i++) {
		pthread_join(threads[i], NULL);
	}
	printf("Everything finished...\n");

	return 0;
}
