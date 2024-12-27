#pragma once

#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#endif

#define PORT 8080

#ifdef _WIN32
int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
#else
int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
#endif

void bind_socket(int server_fd) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        exit(-1);
    }
}

void listen_for_connections(int server_fd) {
        if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed!" << std::endl;
        exit(-1);
    }
}


int accept_connection(int server_fd) {
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    return accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
}

void close_socket(int socket_fd) {
#ifdef _WIN32
    closesocket(socket_fd);
#else
    close(socket_fd);
#endif
}