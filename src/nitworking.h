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

namespace NW {


class Socket {
    sock fd_;
public:
    explicit Socket(sock fd = INVALID_SOCKET_HANDLE);
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    explicit operator bool() const { return fd_ != INVALID_SOCKET_HANDLE; }
    operator sock() const { return fd_; }
    void close();
};

class Server {
public:
    void init(const std::string ip, int port);
    void listen();
private:
    Socket m_server_socket;
    int m_port;
    const std::string m_ip;
};

struct PathMapping {
    std::string path;
    std::string value;
};

#ifdef _WIN32
inline void initialize_winsock();
inline void cleanup_winsock();
#endif

Socket create_server_socket();
void bind_socket(Socket& server, const std::string& ip, int port);
void listen_for_connections(Socket& server);
Socket accept_connection(Socket& server);
void html_buffer(const Socket& client, const std::vector<PathMapping>& mappings);
std::string html_from_file(const char* path);

inline int BUFFER_SIZE = 1024;

}