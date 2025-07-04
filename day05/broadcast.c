#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[]) {
	int send_sock;
	struct sockaddr_in broad_addr;
	int so_brd = 1; // 브로드캐스트 옵셜 활성화를 위한 변수

	if(argc != 3){
		printf("Usage : %s <Port> <Message>\n", argv[0]);
		exit(1);
	}

	// 1. UDP 소켓 생성
	send_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(send_sock == -1){
		error_handling("socket() error");
	} 

	// 2. 브로드캐스트 주소 정보 설정
	memset(&broad_addr, 0, sizeof(broad_addr));
	broad_addr.sin_family = AF_INET;
	broad_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // 브로드캐스트 주소
	broad_addr.sin_port = htons(atoi(argv[1])); // 포트 번호

	// 3. SO_BROADCAST 소켓 옵션 활성화
	// 이 옵션을 켜야 브로드캐스트가 가능합니다.
	if(setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&so_brd, sizeof(so_brd)) == -1){
		error_handling("setsockopt() error");
	}

	// 4. 메시지 전송
	if(sendto(send_sock, argv[2], strlen(argv[2]), 0, (struct sockaddr*)&broad_addr, sizeof(broad_addr)) == -1){
		error_handling("sendto() error");
	}

	printf("Broadcast message sent: %s\n", argv[2]);
	close(send_sock);
	return 0;
}

void error_handling(char *message){
	perror(message);
	exit(1);
}
