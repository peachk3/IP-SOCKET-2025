#include <stdio.h>			// 표준 입출력 함수 (printf, fopen 등)
#include <stdlib.h>			// 표준 라이브러리 (malloc, exit 등)
#include <string.h>			// 문자열 처리 함수 (str_len, strstr 등)
#include <unistd.h>			// 유닉스 시스템 콜 (close 등)
#include <sys/socket.h>		// 소켓 함수들 (socket, bind, listen 등)
#include <netinet/in.h>		// 인터넷 주소 구조체(sockaddr_in)

const char* webpage[] = {
    "HTTP/1.1 200 OK\r\n",			// HTTP 응답 상태 (성공)
    "Server:Linux Web Server\r\n",	// 서버 정보
    "Content-Type: text/html; charset=UTF-8\r\n",	// 내용 타입과 인코딩
    "\r\n",							// 헤더와 본문 구분
    "<!DOCTYPE html>\r\n",			// HTML5 문서 타입 
    "<html><head><title>PeachK Web Page</title>\r\n", // HTML 시작, 제목
    "<style>body{background-color: #FFF000 } </style></head>\r\n",	// CSS 스타일
    "<body><center><h1>Hello PeachK website!</h1><br>\r\n",		// 본문 시작
    "<h2>DoGyoung Park</h2><br>\r\n"
    "<img src=\"Dog.jpg\"></center></body></html>\r\n" // 이미지 태그, HTML 끝
};

// 메인 함수 시작
int main() {
    int server_socket, client_socket;				// 소켓 파일 디스크립터
    struct sockaddr_in server_addr, client_addr;	// 서버/클라이언트 주소 구조체
    socklen_t client_len = sizeof(client_addr);		// 클라이언트 주소 크기
    char buffer[1024];								// 클라이언트 요청 저장 버퍼
    
    // 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET : IPv4 인터넷 프로토콜
	// SOCK_STREAM : TCP 연결(신뢰성 있는 연결)
	// 0: 기본 프로토콜 사용
    
    // 서버 주소 설정
    server_addr.sin_family = AF_INET;			// IPv4 사용
    server_addr.sin_addr.s_addr = INADDR_ANY;	// 모든 인터페이스에서 접속 허용
    server_addr.sin_port = htons(9090);			// 포트 9090
	// htons(Host To Network Short) : 네트워크 바이트 순서로 변환
    
    // 바인드 및 리슨
	// 바인드 : 소켓을 특정 주소와 포트에 연결
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	// 리슨 : 연결 요청 대기 상태로 전환(최대 5개 연결 대기)
    listen(server_socket, 5);
    
    while(1) { // 무한 루프로 계속 클라이언트 요청 처리
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
		// accept() : 클라이언트 연결 요청을 수락하고 새 소켓 반환
		// 이 소켓으로 해당 클라이언트와 통신
        
        // 클라이언트 요청 읽기
        int bytes_received = recv(client_socket, buffer, sizeof(buffer)-1, 0);
        buffer[bytes_received] = '\0';  // 문자열 끝 표시
        // recv() : 클라이언트로부터 데이터 수신
		// buffer : 데이터를 저장할 버퍼
		// sizeof(buffer)-1: null 문자 공간 확보
		// 0 : 특별한 플래그 없음
		
        // 요청 분석 및 응답
        if (strstr(buffer, "GET / ") != NULL) {
			// strstr() : 문자열에서 특정 문자열 찾기
			// "GET / "이 있으면 메인 페이지 요청
			
            // 메인 페이지 요청
            for(int i = 0; i < 9; i++) {
                send(client_socket, webpage[i], strlen(webpage[i]), 0);
            }
			// 배열에 저장된 HTML 페이지를 순서대로 전송
        }
        else if (strstr(buffer, "GET /Dog.jpg") != NULL) {
            // 이미지 파일 요청
            FILE *img_file = fopen("Dog.jpg", "rb");
			// "rb" : 바이너리 읽기 모드로 파일 열기
            if (img_file) {
                // HTTP 헤더 전송
                send(client_socket, "HTTP/1.1 200 OK\r\n", 17, 0);
                send(client_socket, "Content-Type: image/jpeg\r\n", 26, 0);
                send(client_socket, "\r\n", 2, 0);
                
                // 이미지 데이터 전송
                char img_buffer[1024];
                size_t bytes_read;
                while ((bytes_read = fread(img_buffer, 1, sizeof(img_buffer), img_file)) > 0) {
                    send(client_socket, img_buffer, bytes_read, 0);
                }
				// fread() : 파일에서 데이터 읽기
				// 1024바이트씩 읽어서 클라이언트에 전송
				
                fclose(img_file);
            } else {
                // 파일이 없을 경우 404 에러
                send(client_socket, "HTTP/1.1 404 Not Found\r\n\r\n", 26, 0);
            }
        }
        
        close(client_socket); // 클라이언트와 연결 종료
    }
    
    close(server_socket); // 서버 소켓 종료
    return 0;
}
