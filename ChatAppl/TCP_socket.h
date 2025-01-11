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
int TCP_bind_socket(int sockfd, int port);

// Listen for connection on socket fd
int TCP_listen_for_connection(int sockfd);

// Accept the connect and return connection file desciption
int TCP_accept_connection(int sockfd, int *port, char *client_ip);

// Connect to sever by ip and port
int TCP_connect(int sockfd, const char *ip, int port);

// Send message
void TCP_send_message(int sockfd, const char *message);

// Receive message
int TCP_receive_message(int sockfd, char *buffer, uint32_t size);

// Close connect
void TCP_close_connection(int sockfd);

#endif