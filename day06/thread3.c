#include <stdio.h>
#include <pthread.h>

void* thread_main(void*);
int sum = 0; // 전역변수로 선언해줘야 함
int main()
{
	pthread_t t1, t2;
	int range1[] = {1, 5};
	int range2[] = {6, 10};
	
	pthread_create(&t1, NULL, thread_main, (void*)range1);
	pthread_create(&t2, NULL, thread_main, (void*)range2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("result : %d\n", sum);

	return 0;
}
void* thread_main(void* arg)
{
	int start = ((int*)arg)[0];
	int end = ((int*)arg)[1];
	
	while(start <= end){
		sum += start;
		start++;
	}
	return NULL;
}
