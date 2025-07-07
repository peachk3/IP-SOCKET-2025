#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void* thread_main(void*);

int main()
{
	pthread_t t_id;
	int thread_arg = 5;
	void* thread_ret;
	
	if(pthread_create(&t_id, NULL, thread_main, (void*) &thread_arg) != 0) {
		puts("pthread_create() error");
		return -1;
	}	
	// 기다릴 스레드와 반환값을 가짐
	if(pthread_join(t_id, &thread_ret) != 0)
	{
		puts("pthread_join error");
		return -1;
	}
	printf("Thread return message: %s\n", (char*)thread_ret);
	free(thread_ret);
	
	return 0;
}

void* thread_main(void* arg) {
	int i;
	int cnt = *((int*)arg);
	char* meg = (char*)malloc(sizeof(char) * 50);
	strcpy(meg, "Hello, i'm thread!\n");

	for(i = 0; i < cnt; i++){
		sleep(1);
		puts("running thread");
	}
	return (void*)meg;
}

