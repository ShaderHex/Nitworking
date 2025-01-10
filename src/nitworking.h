#pragma once

#include <iostream>
#include <sstream>
#include <vector>

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
const int BUFFER_SIZE = 1024;

struct PathMapping {
    const char* path;
    const char* value;
};

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
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(-1);
    }

    char opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
        std::cerr << "Failed to set SO_REUSEADDR: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        exit(-1);
    }

    return server_fd;
}
#else
int create_server_socket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        exit(-1);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(-1);
    }

    return server_fd;
}
#endif


#ifdef _WIN32
void bind_socket(int server_fd, const std::string& ip_addr, int port_input) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_input);

    if (inet_pton(AF_INET, ip_addr.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip_addr << std::endl;
        close(server_fd);
        exit(-1);
    }

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(-1);
    }
}
#else
void bind_socket(int server_fd, const std::string& ip_addr, int port_input) {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_input);

    if (inet_pton(AF_INET, ip_addr.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip_addr << std::endl;
        close(server_fd);
        exit(-1);
    }

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(-1);
    }
}
#endif


#ifdef _WIN32
void listen_for_connections(int server_fd) {
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        exit(-1);
    }
    std::cout << "Listening for connections..." << std::endl;
}
#else
void listen_for_connections(int server_fd) {
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(-1);
    }
    std::cout << "Listening for connections..." << std::endl;
}
#endif

int accept_connection(int server_fd) {
    sockaddr_in client_addr;
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
        return -1;
    }

    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    std::cout << "Connection accepted from: " << client_ip << std::endl;
    
    return client_fd;
}

std::string get_request_path(const char* request) {
    std::istringstream request_stream(request);
    std::string method, path, version;

    request_stream >> method >> path >> version;

    return path;
}

void html_buffer(int client_fd, const char* html_code, PathMapping* mappings, int num_paths) {
    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = recv(client_fd, buffer, BUFFER_SIZE, 0);
    
    if (bytesRead > 0) {
        std::cout << "Received request:\n" << buffer << std::endl;

        std::string request_path = get_request_path(buffer);
        const char* response_body = "<html><body><h1>404 Not Found</h1></body></html>";

        for (int i = 0; i < num_paths; ++i) {
            if (request_path == mappings[i].path) {
                response_body = mappings[i].value;
                break;
            }
        }

        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(strlen(response_body)) + "\r\n"
            "\r\n" +
            std::string(response_body);

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
    if (closesocket(socket_fd) == SOCKET_ERROR) {
        std::cerr << "Error closing socket: " << WSAGetLastError() << std::endl;
    }
    WSACleanup();
#else
    if (close(socket_fd) < 0) {
        std::cerr << "Error closing socket: " << strerror(errno) << std::endl;
    }
#endif
}

