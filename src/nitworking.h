#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET sock;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
typedef int sock;
#define INVALID_SOCKET_HANDLE -1
#endif

class Socket {
    sock fd_;
public:
    Socket(sock fd = INVALID_SOCKET_HANDLE) : fd_(fd) {}
    ~Socket();

    // Delete copy operations
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // Move operations
    Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = INVALID_SOCKET_HANDLE; }
    Socket& operator=(Socket&& other) noexcept;

    operator sock() const { return fd_; }
    void close();
};

struct PathMapping {
    std::string path;
    std::string value;
};

#ifdef _WIN32
void initialize_winsock();
void cleanup_winsock();
#else
inline void initialize_winsock() {}
inline void cleanup_winsock() {}
#endif

Socket create_server_socket();
void bind_socket(Socket& server, const std::string& ip, int port);
void listen_for_connections(Socket& server);
Socket accept_connection(Socket& server);
void html_buffer(const Socket& client, const std::vector<PathMapping>& mappings);
std::string html_from_file(const char* path);

inline int BUFFER_SIZE = 1024;