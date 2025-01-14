#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#define SERVER_IP       "127.0.0.1"
#define BUFFER_SIZE     1024

// Create and return socket file desciption
int TCP_create_socket();

// Bind a socket with port
int TCP_bind_socket(int sock_fd, int port);

// Listen for connection on socket fd
int TCP_listen_for_connection(int sock_fd);

// Accept the connect and return connection file desciption
int TCP_accept_connection(int sock_fd, int *port, char *client_ip);

// Connect to server by ip and port
int TCP_connect(int sock_fd, const char *ip, int port);

// Send message
int TCP_send_message(int sock_fd, const char *message);

// Receive message
int TCP_receive_message(int sock_fd, char *buffer, uint32_t size);

// Close connect
int TCP_close_connection(int sock_fd);

#endif