#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> // socket
#include <sys/types.h>  // 基本数据类型
#include <unistd.h>     // read write
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> // open close
#include <sys/shm.h>

#include <signal.h>

#define PORT 8888
#define SERV "0.0.0.0"
#define BACKLOG 20 // 最大并发数量
#define BUFF_SIZE 1024

typedef struct doc_type{
	char *key;
	char *value;
} HTTP_CONTENT_TYPE;

// HTTP_CONTENT_TYPE http_content_type[] = {
//         { "html","text/html" },
//         { "gif" ,"image/gif" },
//         { "jpeg","image/jpeg" }
// };

char *SERVER_INFO = "HTTP/1.1 200 OK\r\n"
		    "Server: Apache Server V1.0\r\n"
		    "Accept-Ranges: bytes\r\n"
		    "Content-Length: %d\r\n"
		    "Connection: close\r\n"
		    "Content-Type: %s\r\n\r\n";

int sockfd;
void handle_signal(int sign);           // 退出信号处理
void response(int sock_client); 	// http 响应Client 请求
void parse_header(char *buff);

int main()
{
	signal(SIGINT, handle_signal);

	int count = 0; // 计数
	// 定义 socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// 定义 sockaddr_in
	struct sockaddr_in skaddr;
	skaddr.sin_family = AF_INET; // ipv4
	skaddr.sin_port = htons(PORT);
	skaddr.sin_addr.s_addr = inet_addr(SERV);
	// bind，绑定 socket 和 sockaddr_in
	if (bind(sockfd, (struct sockaddr *)&skaddr, sizeof(skaddr)) == -1)
	{
		perror("bind error");
		exit(1);
	}

	// listen，开始添加端口
	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen error");
		exit(1);
	}

	// 客户端信息
	char buff[BUFF_SIZE];
	struct sockaddr_in claddr;
	socklen_t length = sizeof(claddr);

	while (1)
	{
		int sock_client = accept(sockfd, (struct sockaddr *)&claddr, &length);
		printf("%d\n", ++count);
		if (sock_client < 0)
		{
			perror("accept error");
			exit(1);
		}
		memset(buff, 0, sizeof(buff));
		int len = recv(sock_client, buff, sizeof(buff), 0);
		printf("%s\n", buff);
		//parse_header(buff);
		response(sock_client);
		close(sock_client);
	}
	close(sockfd);
	return 0;
}

void parse_header(char *buff) {
	HTTP_CONTENT_TYPE http_content_type[0x80];
	char *p;
	unsigned int line = 0;
	// 	char * http_type = strsep(&buff, "\r\n");
	//	printf("http_type:%s\n",http_type);
	// 	char * http_header = strsep(&buff, "\r\n\r\n");
	//	printf("http_header:%s\n",http_header);
	//char * http_body = strsep(&buff, "\r\n");
	//printf("http_body:%s\n",http_body);
	while(p = strsep(&buff, "\r\n")) {
		//if(line == 0) {
		//	line++;
		//	printf("%s\n",p);
		//	continue;
		//}
		printf("line = %d\n", line++);
		printf("%s\n", p);
	}
	printf("line = %d\n", line);
}

void response(int sock_client) {
	char HTTP_HEADER[BUFF_SIZE], HTTP_INFO[BUFF_SIZE];
	char *content = "{\"code\":200,\"msg\":\"success\",\"data\":\"you will be success!\"}";
	int len = strlen(content);
	sprintf(HTTP_HEADER, SERVER_INFO, len, "application/json");
	len = sprintf(HTTP_INFO, "%s%s", HTTP_HEADER, content);

	send(sock_client, HTTP_INFO, len, 0);
}

void handle_signal(int sign)
{
	printf("\nSIGNAL INTERRUPT\n");
	close(sockfd);
	exit(0);
}
