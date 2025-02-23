#include "nitworking.h"
#include <iostream>
#include <vector>

int main() {
#ifdef _WIN32
    initialize_winsock();
#endif

    try {
        std::vector<PathMapping> mappings = {
            {"/home", html_from_file("example.html")},
            {"/about", "<html><body>Made by ShaderHex</body></html>"},
            {"/contact", "<html><body>No Discord server</body></html>"}
        };

        Socket server = create_server_socket();
        bind_socket(server, "0.0.0.0", 8080);
        listen_for_connections(server);
        std::cout << "Listening on port 8080\n";

        while (true) {
            Socket client = accept_connection(server);
            html_buffer(client, mappings);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}