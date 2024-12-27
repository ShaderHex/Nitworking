#include "nitworking.h"

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    create_server_socket();
    bind_socket(server_fd);
    listen_for_connections(0);
}