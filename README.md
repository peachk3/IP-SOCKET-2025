# IP-SOCKET-2025
IP/SOCKET

## 4일차
멀티 플레싱 (p.39 ~ )- select

멀티플레싱 - select
- select 함수는 다수의 파일 디스크립터를 등록하여 읽기/쓰기/오류에 대한 이벤트 발생 여부를 관찰

```C
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, const struct timeval* timeout);
    => 타임아웃 시 0, 변화 감시 시는 변화 감지된 파일 디스크립터 수, 실패 시 -1 반환
```
- maxfd : 검사 대상이 되는 파일 디스크립터의 수
- readset : fd_set형 변수에 수신된 데이터의 존재여부에 관심있는 파일 디스크립터
- writeset : fd_set형 변수에 블로킹 없는 데이터의 전송여부에 관심있는 파일 디스크립터
- exceptset	: fd_set형 변수에 예외상황 발생여부에 관심있는 파일 디스크립터
- tmeout : select 함수 호출 이후에 무한정 블로킹에 빠지지 않도록 타임아웃 설정
- 반환값 : 타임아웃에 의한 반환 시는 0, 변화 발생시는 변화된 파일 디스크립터의 수를 반환

```C
struct timeval {
    long tv_set;    		// seconds
    long tv_usec;			// microseconds
}
```
- Timeval구조체는 gettimeofday() 함수로 현재 시간을 구하여 저장


멀티플레싱 - epoll

<!-- <img src="./image/0001.png" width="600"> -->


방화벽 인바운드 규칙 생성 > 팀 서버 생성 > 포트 번호 지정 (TCP/18080)
    <img src="./image/0003.png" width="600">



서버 실행
./echo_selectsv 18080[서버포트 번호]

- 다른 사람의 ip 주소 서버로 접속
./echo_client [ip주소] [서버포트번호]

오류 발생
```shell
    bind() error
```
-> 발생시 현재 사용중인 서버 프로세스가 있는 것
=> kill -9 프로세스명(PID)

결과 
- 클라이언트에서 보내는 메시지 서버에서 출력
    <img src="./image/0002.png" width="600">




## 5일차

