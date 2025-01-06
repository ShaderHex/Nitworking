#pragma once

#include <iostream>

#ifdef _WIN32
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fstream>
#endif

#ifdef _WIN32
typedef SOCEKT sock;
#else
typedef int sock;
#endif
//#define PORT 8080
const int BUFFER_SIZE = 1024;

#ifdef _WIN32
void initialize_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        exit(-1);
    }
}
#endif

#ifdef _WIN32
int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
#else
int create_server_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
#endif

#ifdef _WIN32
void bind_socket(int server_fd, int port_input) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_input);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return exit(-1);
    }
}
#else
void bind_socket(int server_fd, int port_input) {
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_input);

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SO_ERROR) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return exit(-1);
  }  
}
#endif

#ifdef _WIN32
void listen_for_connections(int server_fd) {
        if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return exit(-1);
    }
}
#else
void listen_for_connections(int server_fd) {
        if (listen(server_fd, SOMAXCONN) == SO_ERROR) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        return exit(-1);
    }
}
#endif

#ifdef _WIN32
void client_socket(int server_fd, int client_fd) {
    sockaddr_in client_addr;
    int clientAddrSize = sizeof(client_addr);
    if (client_fd == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        exit(-1);
    }
}
#else
void client_socket(int server_fd, int client_fd) {
    sockaddr_in client_addr;
    int clientAddrSize = sizeof(client_addr);
    if (client_fd == -1) {
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(-1);
    }
}
#endif

int accept_connection(int server_fd) {
    sockaddr_in client_addr;
    // Use socklen_t on Linux and int on Windows
#ifdef _WIN32
    int clientAddrSize = sizeof(client_addr);
#else
    socklen_t clientAddrSize = sizeof(client_addr);
#endif

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &clientAddrSize);
    if (client_fd < 0) {
#ifdef _WIN32
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
#else
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
#endif
        return -1;  // Indicate failure
    }

    std::cout << "Connection accepted from: "
              << inet_ntoa(client_addr.sin_addr) << std::endl;
    return client_fd;
}

void html_buffer(int client_fd, const char* html_code) {
    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytesRead > 0 ) {
        std::cout << "Received request:\n" << buffer << std::endl;

        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(strlen(html_code)) + "\r\n"
            "\r\n" +
            std::string(html_code);

            send(client_fd, http_response.c_str(), http_response.size(), 0);
    }
}

void change_buffer_size(int set_buffer_size) {
    #undef BUFFER_SIZE
    #define BUFFER_SIZE set_buffer_size
}

const char* html_from_file(const char* path_to_html) {
    std::ifstream file(path_to_html, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    if (size <= 0) {
        std::cerr << "File is empty or inaccessible." << std::endl;
        return nullptr;
    }

    file.seekg(0, std::ios::beg);

    char* html_code = new char[size + 1];
    if (!file.read(html_code, size)) {
        std::cerr << "Error reading file." << std::endl;
        delete[] html_code;
        return nullptr;
    }

    html_code[size] = '\0';
    file.close();

    return html_code;
}

void close_socket(int socket_fd) {
#ifdef _WIN32
    closesocket(socket_fd);
#else
    close(socket_fd);
#endif
}
