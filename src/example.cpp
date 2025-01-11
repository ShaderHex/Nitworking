#include "nitworking.h"
#include <iostream>
#include <map>

int main() {
#ifdef _WIN32
    initialize_winsock();
#endif

    PathMapping mapping[] = {
        {"/home", html_from_file("example.html")},
        {"/about", "<html><body>Made by ShaderHex (on github)</body></html>"},
        {"/contact", "<html><body>This project doesn't have discord server apparently</body></html>"}
    };

    sock server_fd = create_server_socket();
    unsigned int port = 8080;

    bind_socket(server_fd, "127.0.0.1", port);

    listen_for_connections(server_fd);
    std::cout << "Listening on port " << port << std::endl;

    while (true) {
        sock client_fd = accept_connection(server_fd);
                
        html_buffer(client_fd, html_from_file("example.html"), mapping, 3);
        
        close_socket(client_fd);
    }

    close_socket(server_fd);
}
