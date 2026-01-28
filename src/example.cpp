#include "nitworking.h"
#include <vector>

int main() {
#ifdef _WIN32
    NW::initialize_winsock();
#endif

    try {
        std::vector<NW::PathMapping> mappings = {
            {"/home", NW::html_from_file("example.html")},
            {"/about", "<html><body>Made by ShaderHex</body></html>"},
            {"/contact", "<html><body>No Discord server</body></html>"}
        };

        NW::Socket server = NW::create_server_socket();
        NW::bind_socket(server, "127.0.0.1", 8080);
        NW::listen_for_connections(server);
        std::cout << "Listening on port 8080\n";

        while (true) {
            NW::Socket client = accept_connection(server);
            if (client) {
                html_buffer(client, mappings);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

#ifdef _WIN32
    cleanup_winsock();
#endif
    return 0;
}