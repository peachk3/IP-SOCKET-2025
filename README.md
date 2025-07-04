# IP-SOCKET-2025
IP/SOCKET

## 4일차
### IO 멀티 플레싱 (IO Multiplexing)
##### 멀티프로세서의 단점
- 프로세서의 빈번한 생성 
-> 성능저하 발생, 멀티 프로세서의 흐름 구현 어려움
- 해결책
=> 하나의 프로세서로 다수의 클라이언트에게 서비스를 제공!
=> IO 멀티 플렉싱(하나의 프로세서가 서버 소켓 + 여러 클라이언트 소켓을 관리)

- fd_set 구조체
    - fd를 관리하기 위해 디자인 된 구조체
    - fd_set 변수를 사용하는 매크로 함수

    |함수|설명|
    |----|----|
    |fd_zero(fd_set* fdset)|fdset 테이블의 모든 비트를 0으로 초기화|
    |fe_set(in fd, fd_set* fdset)|fdset테이블에 전달된 fd 등록|
    |fe_clr(int fd, fd_set* fdset)| fdset 테이블에 전달된 fd 삭제|
    |fd_isset(int fd, fd_set* fdset)| fdset 테이블에 전달된 fd의 정보가 있으면 1 반환|


#### select
- 다수의 파일 디스크립터를 등록하여 읽기/쓰기/오류에 대한 이벤트 발생 여부를 관찰

```C
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, const struct timeval* timeout);
    => 타임아웃 시 0, 변화 감시 시는 변화 감지된 파일 디스크립터 수, 실패 시 -1 반환
```
|항목|설명|
|-----------|--------------|
|maxfd|검사 대상이 되는 파일 디스크립터의 수 |
|readset |fd_set형 변수에 수신된 데이터의 존재여부에 관심있는 파일 디스크립터|
|writeset| fd_set형 변수에 블로킹 없는 데이터의 전송여부에 관심있는 파일 디스크립터|
| exceptset|fd_set형 변수에 예외상황 발생여부에 관심있는 파일 디스크립터 |
|timeout |select 함수 호출 이후에 무한정 블로킹에 빠지지 않도록 타임아웃 설정|
|반환값|타임아웃에 의한 반환 시는 0, 변화 발생시는 변화된 파일 디스크립터의 수를 반환|


```C
struct timeval {
    long tv_set;    		// seconds
    long tv_usec;			// microseconds
}
```
- Timeval구조체는 gettimeofday() 함수로 현재 시간을 구하여 저장
- 시간을 표시하기 위한 구조체


<!-- <img src="./image/0001.png" width="600"> -->


방화벽 인바운드 규칙 생성 > 팀 서버 생성 > 포트 번호 지정 (TCP/18080)
    <img src="./image/0003.png" width="600">


* 서버 실행
```shell
./echo_selectsv 18080[서버포트 번호]
```

* 다른 사람의 ip 주소 서버로 접속
```shell
./echo_client [ip주소] [서버포트번호]
```

* 오류
```shell
    bind() error  // 오류 발생
    // 현재 사용중인 서버 프로세스가 있는 것!
    //------------------------
    sudo lsof -i :[포트번호]
    kill -9 프로세스명(PID) // 강제 종료
```

결과 
- 클라이언트에서 보내는 메시지 서버에서 출력
    <img src="./image/0002.png" width="600">



## 5일차
### 멀티플레싱 - poll
- select 보다 유연하고 구조체 배열로 처리 (FD 수 제한 X)

```C
#include <poll.h>
int poll(struct pollfd fds[], nfds t nfds, int timeout);

struct pollfd {
    int fd;         // file descriptor 
    short events;   // requested events (관찰할 이벤트) 
    short revents;  // returned events (실제 발생한 이벤트)
}
```
nfds : fds[]의 크기
timeout : 대기시간(-1은 무한 대기, 0은 즉시 리턴)

* 파일 디스크립터를 등록하고 감시할 이벤트 지정
* Revents에는 감시할 이벤트 중 발생한 이벤트를 저장하여 사용

|이벤트 상수 | 설명 |
|------------|-----|
|POLLIN| 읽기 이벤트 감시|
|POLLOUT| 쓰기 이벤트 감시|
|POLLERR| 에러 감지|
|POLLUP| 연결 종료 감지|

* fds[]에 server socket을 등록하고 읽기 이벤트가 발생하면 client socket 을 생성
-> client socket을 등록하고 읽기 이벤트 발생시 데이터 read


#### 멀티플레싱 - epoll
- 리눅스에서 고성능 네트워크 서버 구현에 사용되는 이벤트 기반 멀티 플레싱 서버로 성능과 확장성이 우수하며, 대규모 동시 접속 처리에 적합 (생성 -> 설정 -> 대기)

```C
    epoll_ctl(A, EPOLL_CTL_ADD, B, C); // epoll 인스턴스 A에 파일 디스크립터 B를 등록하여 C를 통해 전달된 이벤트를 관찰
```

```C
    struct epoll_event{
        uint32_t events // 감시할 이벤트
        epoll_data_t data; // fd 또는 사용자 정의 이벤트
    }
```

##### epoll_wait() 
- 관심있는 fd들에 무슨 일이 일어났는지를 조사,
사건이 일어나면 (epoll_event).events[] 리스트에 일어난 사건들을 저장.
동시 접속사수에 상관없이 maxevents 파라미터로 최대 몇 개까지의 event를 처리할 것인지 지정해주고 있음
```c
for (int i = 0; i < n; i++) {
    int fd = events[i].data.fd;					// 이벤트가 일어난 fd
        uint32_t ev = events[i].events;				// 일어난 이벤트 종류 

        if (ev & EPOLLIN) {							// 일어난 이벤트가 EPOLLIN이면
                printf("읽기 이벤트 발생: fd = %d\n", fd);
        }
}
```
* epoll 함수 사용
<img src="./image/0005.png" width="600">

| 항목        | Level-Triggered                                                                 | Edge-Triggered                                                                                       |
|-------------|----------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------|
| 이벤트 발생 | 읽을 게 있으면 **계속 이벤트를 발생**<br>데이터가 들어올 때마다 `epoll_wait()` 발생하고 읽을 때까지 알림 | 상태 변화가 있을 때만 **한 번 발생**<br>새 데이터가 처음 들어올 때 **한 번만 알림**<br>`read()`로 버퍼를 완전히 비우지 않으면 데이터 유실이 일어난다. 따라서 반드시 루프 + non-blocking 소켓 + 전체 버퍼 읽기 |
| 반복 여부   | 필요 없음                                                                       | 필요함                                                                                                |
| 복잡도     | 구현 쉬움                                                                        | 구현 복잡                                                                                              |
| 효율성     | 낮음 (CPU 사용 비효율적)                                                        | 높음 (CPU 사용 효율적)                                                                                |
| 디폴트 모드 | 기본값                                                                           | `EPOLLET` 명시 필요                                                                                   |
| 버퍼        | 다음 `epoll_wait()`에서도 또 알림                                               | 알림 없음 → 데이터 유실 가능                                                                          |
| 예시        | `Events = EPOLLIN`<br>`epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);`               | `Events = EPOLLINㅣEPOLLET`<br>`epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);`                               |


### UDP
- 흐름제어가 없음
- 서버와 클라이언트가 연결되어 있지 않음
- 데이터 입출력 함수(sendto, recvfrom)
    - sendto 함수 호출 이전에 ip와 port가 할당되어야 함 (자동 할당)
```C
매번 호출 시 주소 정보를 저장한다.
#include <sys/socket.h>

ssiz_t sendto(int sock, void *buf, size_t nbytes, int flags, struct sockaddr *to, socklent_t addrlen);
    ⇒ 성공 시 전송 바이트 수, 실패 시 -1 반환
    
ssiz_t recvfrom(int sock, void *buf, size_t nbytes, int flags, struct sockaddr *from, socklent_t addrlen)
    ⇒ 성공 시 수신된 바이트 수, 실패 시 -1 반환
```
|항목|설명|
|---|---|
|sock|		UDP소켓 파일디스크립터|
|buf|			전송할 버퍼의 주소값|
|nbytes|		전송할 데이터 크기|
|flags|		옵션. 없으면 0|
|to|			목적지 주소정보를 가지고 있는 sockaddr 구조체 변수 주소값|
|addrlen|		목적지 주소정보의 크기|


- 기본적으로 unconnected UDP 소켓
- connected UDP 소켓을 생성하려면 connect 함수를 호출하면 됨


(p.51)
### UDP 기반 멀티캐스트 & 브로드캐스트
- 네트워크에서 한 송신자가 데이터를 한 번에 여러 장치에서 전송할 때 사용되는 방식
- 멀티캐스트
    - 특정 멀티캐스트 그룹을 대상으로 데이터 딱 한 번 전송
    - 그룹에 속하는 클라이언트는 모두 데이터 수신

- 브로드캐스트
    - 동일한 네트워크 내에 존재하는 호스트에게 데이터 전송
    - 데이터 전송의 대상이 호스트가 아닌 네트워크

* 같은 네트워크에 있는 모든 장치에게 알림	 ⇒  브로드캐스트(DHCP 요청 등)
* 같은 서비스를 구독 중인 장치들에게 만 전달	 ⇒ 멀티캐스트(IPTV, 실시같스트리밍, 게임, 모니터링)
* 하나의 대상에게만 전달					 ⇒ 유니캐스트


(p.52)
#### 멀티캐스트
- 1 대 N 통신
- 수신을 확인하지 않는 UDP 방식 사용(연결 개념 X)
- 224.0.0.0 ~ 239.255.255.255 의 범위에서 설정
- TTL(Time to Live)이라는 필드를 가지며 라우터를 지날때마다 TTL값이 1씩 감소함
- TTL은 0이 되면 더 이상 출력되지 않음


* 그룹에 가입하는 방법 (수신자)
```c
Struct  ip_mreq  mreq;
mreq_adr.imr_multiaddr.s_addr = inet_addr(“멀티캐스트 그룹 IP”);
mreq_adr.imr_interface.s_addr = htonl(INADDR_ANY);
seosockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&mreq_adr, sizeof(mreq_adr));
IP프로토콜의 IP_ADD_MEMBERSHIP옵션을 사용한다.
```

* 그룹정보를 저장하는 ip_mreq 구조체
```C
Struct ip_mreq {
    struct  in_addr  imr_multiaddr;
    struct  in_addr  imr_interface;
}
```

* sender는 그룹주소로 전송만 하고 Receiver는 그룹에 가입하는 코드가 추가되어야 함


1. 패킷의 무한 루프 방지
    - 네트워크에 라우팅 설정 오류 등으로 인해 루프 구간이 생길 경우, 패킷이 해당 구간에 맴돌며 네트워크 전체 마비 우려 있음
    => TTL은 패킷이 라우터를 하나 거칠때마다 1씩 감소하고 0이 되면 해당 패킷을 소멸하여 무한정 네트워크에 남아있지 않게 안정성 보장

2. 멀티캐스트 전송 범위 제어
    - 브로드캐스트는 기본적으로 아우터를 통과하지 못하고 하나의 로컬 내에서만 전파됨
    - 멀티캐스트는 라우터를 통과하여 여러 다른 네트워크를 전송될 수 있게 설계됨 -> 아무런 제약이 없을 경우 전 세계 인터넷망으로 퍼져 트래픽 유발 가능성 있음
    => TTL은 이 멀티캐스트 패킷이 얼마나 멀리 퍼져 나갈지를 제어하는 핵심 수단
    TTL = 1 로컬 네트워크 내
    TTL = 16 캠퍼스, 특정 기관 내부 네트워크 정도의 범위
    TTL = 64 지역(Region)단위의 네트워크 범위
    TTL = 128 전 세계적인 범위


* 멀티캐스트 전송 결과
    <img src="./image/0007.png" width="600">
    - 두 군데 전송 가능
    - 한 번 전송 후 다시 연결 후 에 전송해야 함!


#### 브로드캐스트
- 같은 서브넷의 모든 호스트를 대상으로 함
- IP주소가 255.255.255.255 또는 서브넷 브로드캐스트
- 라우터를 통과하지 못해 로컬에서만 유효함
- 전송자 옵션
```C
int flag = 1;
setsocket(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flaf):
```
|항목|설명|
|----|----|
|SO_BROADCAST|브로드캐스트 전송 허용|
|Flag|1로 설정하면 255.255.255.255 같은 브로드캐스트 주소로 UDP 전송 가능
이 설정이 없으면 sendto()전송 시 permission 오류 발생|


* 브로드캐스트 전송 결과
    <img src="./image/0008.png" width="600">
 - 수신자 먼저 오픈 > 송신자 메시지 전송
