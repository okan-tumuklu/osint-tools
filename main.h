#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

// todo: add ipv6 support later
void get_block(char *target) {
    struct addrinfo hints = {0}, *res;
    char ip[16];
    int sock;
    struct sockaddr_in serv;
    struct hostent *he;
    char req[64], buf[1024];

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(target, NULL, &hints, &res) != 0) {
        printf("[-] dns patladi\n");
        return;
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip, sizeof(ip));
    freeaddrinfo(res);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    he = gethostbyname("whois.radb.net");
    if (!he) {
        printf("[-] whois sunucusu bulunamadi\n");
        close(sock);
        return;
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(43);
    serv.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    sprintf(req, "%s\r\n", ip); // snprintf ugrasmaya gerek yok
    send(sock, req, strlen(req), 0);

    printf("[*] IP: %s\n", ip);

    int bytes = 0;
    while ((bytes = recv(sock, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[bytes] = '\0';
        
        char *route = strstr(buf, "route:");
        if (route) {
            char range[32];
            sscanf(route, "route: %s", range);
            printf("[+] BGP: %s\n", range);
            break; 
        }
        memset(buf, 0, sizeof(buf)); // garanti olsun diye sifirla
    }

    close(sock);
}
