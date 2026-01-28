#include "nitworking.h"
#include <fstream>
#include <sstream>
#include <cstring>


namespace NW {

// Socket implementation
Socket::Socket(sock fd) : fd_(fd) {}

Socket::~Socket() {
    close();
}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = INVALID_SOCKET_HANDLE;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = INVALID_SOCKET_HANDLE;
    }
    return *this;
}

void Socket::close() {
    if (fd_ != INVALID_SOCKET_HANDLE) {
#ifdef _WIN32
        closesocket(fd_);
#else
        ::close(fd_);
#endif
        fd_ = INVALID_SOCKET_HANDLE;
    }
}

#ifdef _WIN32
// Initializes Windows sockets. It is necessary only on Windows platforms.
void initialize_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
}

void cleanup_winsock() {
    WSACleanup();
}
#endif

// This function creates a server socket and configures it for connection acceptance:
// INVALID_SOCKET is returned on failure.
Socket create_server_socket() {
    sock fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET_HANDLE) {
#ifdef _WIN32
        throw std::runtime_error("Socket creation failed: " + std::to_string(WSAGetLastError()));
#else
        throw std::runtime_error(std::string("Socket creation failed: ") + strerror(errno));
#endif
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                  reinterpret_cast<const char*>(&opt), sizeof(opt)) == -1) {
#ifdef _WIN32
        closesocket(fd);
#else
        ::close(fd);
#endif
        throw std::runtime_error("Setsockopt failed");
    }

    return Socket(fd);
}

// Binds the server socket to the specified IP address and port.
void bind_socket(Socket& server, const std::string& ip, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address: " + ip);
    }

    if (bind(server, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == -1) {
#ifdef _WIN32
        throw std::runtime_error("Bind failed: " + std::to_string(WSAGetLastError()));
#else
        throw std::runtime_error(std::string("Bind failed: ") + strerror(errno));
#endif
    }
}

// Prepares the server to listen for incoming client connections.
void listen_for_connections(Socket& server) {
    if (listen(server, SOMAXCONN) == -1) {
#ifdef _WIN32
        throw std::runtime_error("Listen failed: " + std::to_string(WSAGetLastError()));
#else
        throw std::runtime_error(std::string("Listen failed: ") + strerror(errno));
#endif
    }
    std::cout << "Listening for connections...\n";
}

// Accepts an incoming connection from a client and returns the client socket descriptor.
Socket accept_connection(Socket& server) {
    sockaddr_in client_addr{};
#ifdef _WIN32
    int addrlen = sizeof(client_addr);
#else
    socklen_t addrlen = sizeof(client_addr);
#endif

    sock client_fd = accept(server, reinterpret_cast<sockaddr*>(&client_addr), &addrlen);
    if (client_fd == INVALID_SOCKET_HANDLE) {
#ifdef _WIN32
        throw std::runtime_error("Accept failed: " + std::to_string(WSAGetLastError()));
#else
        throw std::runtime_error(std::string("Accept failed: ") + strerror(errno));
#endif
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "Connection from: " << client_ip << "\n";

    return Socket(client_fd);
}

std::string html_from_file(const char* path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + std::string(path));
    }

    size_t file_size = file.tellg();
    std::string content(file_size, '\0');
    file.seekg(0);
    file.read(&content[0], file_size);

    return content;
}

// Handles the client's request and serves the appropriate HTML response.
void html_buffer(const Socket& client, const std::vector<PathMapping>& mappings) {
    std::string buffer(BUFFER_SIZE, '\0');

#ifdef _WIN32
    int bytes_read = recv(client, &buffer[0], BUFFER_SIZE, 0);
#else
    ssize_t bytes_read = recv(client, &buffer[0], BUFFER_SIZE, 0);
#endif

    if (bytes_read > 0) {
        std::istringstream request(buffer);
        std::string method, path, version;
        request >> method >> path >> version;

        std::string response = "<html><body><h1>404 Not Found</h1></body></html>";
        for (const auto& mapping : mappings) {
            if (path == mapping.path) {
                response = mapping.value;
                break;
            }
        }

        const std::string header = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(response.size()) + "\r\n\r\n";

        send(client, (header + response).c_str(), header.size() + response.size(), 0);
    }
}

void Server::init(const std::string ip, int port) {
    m_port = port;
#ifdef _WIN32
    initialize_winsock();
#endif

    m_server_socket = create_server_socket();
    bind_socket(m_server_socket, ip, m_port);
    listen_for_connections(m_server_socket);
}

void Server::listen() {
    Socket client = accept_connection(m_server_socket);

    std::vector<PathMapping> mappings;
    html_buffer(client, mappings);
}

}