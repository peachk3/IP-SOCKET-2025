#include <stdio.h>
#include <pthread.h>

void* thread_inc(void*);
void* thread_des(void*);

long long num = 0;

int main()
{
	pthread_t thread_id[100];

	for(int i = 0; i < 100; i++){
		if(i % 2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}
	for(int i = 0; i < 100; i++){
		pthread_join(thread_id[i], NULL);
	}
	printf("result : %lld\n", num);
	
	return 0;
}
void* thread_inc(void* arg)
{
	int i;
	for(i = 0; i < 50000000; i++){
		num += 1;
	}
	return NULL;
}
void* thread_des(void* arg)
{
	int i;
	for(i = 0; i < 50000000; i++){
		num -= 1;
	}
	return NULL;
}
