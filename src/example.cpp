#include "nitworking.h"
#include <iostream>

int main() {
#ifdef _WIN32
    initialize_winsock();
#endif
    int server_fd = create_server_socket();

    bind_socket(server_fd);
    listen_for_connections(server_fd);
    std::cout<<"Listening on port "<< PORT <<std::endl;
    
    SOCKET client_fd = accept_connection(server_fd);
    client_socket(server_fd, client_fd);
    while (true) {
        html_buffer(client_fd, "<html><body><h1>Hello, C++ on Windows! But dynamic one!</h1></body></html>");
    }

    close_socket(client_fd);
    close_socket(server_fd);
}