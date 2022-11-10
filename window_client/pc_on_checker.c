#include <stdio.h>
#include <curl/curl.h>

int main(int argc, char *argv)
{
	CURL *curl;
	CURLcode res;

	struct sockaddr_in server_addr;
	curl_socket_t sockfd;

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:1234");

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == CURL_SOCKET_BAD)
		{
			printf("Error creating listening socket.\n");
			return 3;
		}

		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(PORTNUM);

		servaddr.sin_addr.s_addr = inet_addr(IPADDR);
		if (INADDR_NONE == servaddr.sin_addr.s_addr)
		{
			close(sockfd);
			return 2;
		}

		if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) ==
			-1)
		{
			close(sockfd);
			printf("client error: connect: %s\n", strerror(errno));
			return 1;
		}
		curl_easy_cleanup(curl);
		close(sockfd);
	}

	return 0;
}