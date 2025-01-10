#include "nitworking.h"
#include <iostream>

int main() {


#ifdef _WIN32
    initialize_winsock();
#endif
    sock server_fd = create_server_socket();
    unsigned int port = 8080;

    bind_socket(server_fd, "127.0.0.1" , port);
    listen_for_connections(server_fd);
    std::cout<<"Listening on port "<< port <<std::endl;
    const char* paths[] = {"/home", "/about", "/contact"};

    while(true) {
        sock client_fd = accept_connection(server_fd);
        html_buffer(client_fd, html_from_file("example.html"), paths, 3);
        close_socket(client_fd);
    }

    close_socket(server_fd);
}
