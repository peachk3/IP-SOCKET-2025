/*Level Trigger Epoll*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#define MAX_EVENTS 10

int main(int argc, char* argv[]){
	int server_fd, epfd, client_fd, event_cnt;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	struct epoll_event ev, events[MAX_EVENTS]; // epoll 이벤트 구조체 변수, 배열


	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(atoi(argv[1]));
	if(bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd, 5) == -1){
		perror("Listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	epfd = epoll_create1(0); 		/* 1. epoll 인스턴스 생성(관찰대상을 등록) */
	ev.events = EPOLLIN;			// epoll 이벤트 구조체의 events에 발생이벤트 저장
	ev.data.fd = server_fd;		// 관잘할 fd로 server_fd 저장
	/* 2. 서버 fd의 읽기 이벤트를 감지하도록 epfd 인스턴스에 추가 */
	epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

	while(1){
	/* 3. 이벤트 발생 대기, 인스턴스에 이벤트가 발생하면  이벤트 발생 정보를 events []*/
		event_cnt = epoll_wait(epfd, events, MAX_EVENTS, -1); //이벤트 발생 수
	 		if(event_cnt == -1){
			perror("epoll_wait() failed");
			break;
		}
		for (int i = 0; i < event_cnt; i++) { // 발생 이벤트 만큼 반복 처리
			if(events[i].data.fd == server_fd) { // 발생한 이벤트가 서버 fd이면
				client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen); // 클라 [
				ev.events = EPOLLIN;			// 이벤트에 읽기 이벤트 저장
				ev.data.fd = client_fd;		// data.fd에 클라이언트 fd를 저장하여
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev); 	// epoll 인스턴스에 추가 (클라_
				printf("connected client: %d\n", client_fd);
			}
			else { // 발생한 이벤트가 클라이언트 fd이면
				char buffer[1024] = {0};
				int read_len = read(events[i].data.fd, buffer, sizeof(buffer));
				if(read_len <= 0) {		// 읽어온게 없으면
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL); // 인스턴스
					close(events[i].data.fd);	// 클라이언트 fd를 닫는다
					printf("closed client : %d\n", events[i].data.fd);
				}else {	// 에코한다
					printf("client[%d] %s", events[i].data.fd, buffer);
					write(events[i].data.fd, buffer, read_len);
				}
			}
		}
	}
}
