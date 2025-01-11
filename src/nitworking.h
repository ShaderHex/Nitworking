#pragma once
#include "nitworking.cpp"

void initialize_winsock();
int create_server_socket();
void bind_socket(int server_fd, const std::string& ip_addr, int port_input);
void listen_for_connections(int server_fd);
int accept_connection(int server_fd);
void html_buffer(int client_fd, const char* html_code, PathMapping* mappings, int num_paths);
const char* html_from_file(const char* path_to_html);
void close_socket(int socket_fd);