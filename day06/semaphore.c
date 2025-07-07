#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void* arg);
void* accu(void* arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;
	sem_init(&sem_one, 0, 0);			// 초기값 0 : 읽기 먼저, 누적(accu)은 대기
	sem_init(&sem_two, 0, 1);			// 초기값 1 : 읽기 가능 상태

	pthread_create(&id_t1, NULL, read, NULL);	 	// 입력 스레드
	pthread_create(&id_t2, NULL, accu, NULL);		// 누적 스레드

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);

	sem_destroy(&sem_one);
	sem_destroy(&sem_two);
	return 0;
}

void* read(void* arg)
{
	int i;
	for(i = 0; i < 5; i++){
		fputs("Input num: ", stdout);
		sem_wait(&sem_two);			// 세마포어 획득하면 초기값 -1 // 누적 스레드가 이전 숫자 처리할 때까지 대기
		scanf("%d", &num);			// 사용자로부터 숫자 입력
		sem_post(&sem_one);	 		// 세마포어 생성하면 초기값 +1) // 입력 완료 -> 누적 스레드에게 작업 허용
	}	
	return NULL;
}

void* accu(void* arg){
	int sum = 0, i;
	for(i = 0; i < 5; i++){
		sem_wait(&sem_one);			// 세마포어 획득  // 입력이 있을 때까지 대기
		sum += num;						// 공유 변수 num을 누적합에 더함
		sem_post(&sem_two);			// 세마포어 반환		// 다음 입력을 받을 수 있도록 허용
	}
	printf("Result: %d\n", sum);
	return NULL;
}
