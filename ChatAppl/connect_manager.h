#include "TCP_socket.h"
#include <sys/select.h>

#ifndef CONN_MANAGE_H
#define CONN_MANAGE_H

#define MAX_CLIENTS 10

// Structure to store a connection info
typedef struct connect_manager
{
    uint32_t client_id;
    int sock_fd;
    uint32_t in_port;
    char *in_addr;
} connect_manager_t;

// Client list
static connect_manager_t *clients_p[MAX_CLIENTS] = {NULL};

// Bind running program as server
int create_server_socket(int port);

// Create a client socket by IP and port
int create_client_socket(const char *ip, int port);

// Handle new client connection
void accept_new_connection(int listen_fd);

// Handle client communication
void client_handler(void);

// Send message to a connection
void send_to_client(char *message, uint32_t client_id);

// Append new connection to list
static void add_client(connect_manager_t client);

// Remove connection from list and free memory
void remove_client(uint32_t client_id);
void remove_all_clients(void);

// Display the list of active connections
void list_all_connections(void);

// Get the IP and port of the local socket.
void getMyIp(int sock_fd);

#endif