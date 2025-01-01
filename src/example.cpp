#include "nitworking.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {


#ifdef _WIN32
    initialize_winsock();
#endif
    int server_fd = create_server_socket();
    unsigned int port = 8080;

    bind_socket(server_fd, port);
    listen_for_connections(server_fd);
    std::cout<<"Listening on port "<< port <<std::endl;
    html_from_file();
    SOCKET client_fd = accept_connection(server_fd);
    client_socket(server_fd, client_fd);
    change_buffer_size(4026);
    while (true) {
        html_buffer(client_fd, html_from_file());
    }

    close_socket(client_fd);
    close_socket(server_fd);
}