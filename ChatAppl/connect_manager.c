#include "connect_manager.h"

static uint32_t id_counter = 1;

int create_server_socket(int port) {
    int sock_fd = TCP_create_socket();
    TCP_bind_socket(sock_fd, port);
    
    if (TCP_listen_for_connection(sock_fd) >= 0) {
        printf("Listening on port: %d\n", port);
    }

    return sock_fd;
}

int create_client_socket(const char *ip, int port) {
    int sock_fd = TCP_create_socket();

    if (TCP_connect(sock_fd, ip, port) >= 0) {
        connect_manager_t client;
        client.client_id = id_counter++;
        client.in_addr = strdup(ip);
        client.in_port = port;
        client.sock_fd = sock_fd;

        add_client(client);
    }

    return sock_fd;
}

void accept_new_connection(int listen_fd) {
    int client_port;
    char client_ip[INET_ADDRSTRLEN];

    int client_fd = TCP_accept_connection(listen_fd, &client_port, client_ip);

    if (client_fd >= 0) {
        printf("\nNew connection detected\n");
    
        connect_manager_t client;
        client.client_id = id_counter++;
        client.in_addr = client_ip;
        client.in_port = client_port;
        client.sock_fd = client_fd;

        add_client(client);
    }
}

void client_handler(void) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(clients_p[i] != NULL && clients_p[i]->sock_fd > 0) {
            char buffer[BUFFER_SIZE];
            int bytes_received = TCP_receive_message(clients_p[i]->sock_fd, buffer, BUFFER_SIZE);
 
            // Handle client messages
            printf("\n");
            if (bytes_received <= 0) {
                printf("Connection closed %s on port: %d\n", clients_p[i]->in_addr, clients_p[i]->in_port);
                remove_client(clients_p[i]->client_id);
            } else {
                printf("Message received from: %s\n", clients_p[i]->in_addr);
                printf("Port %d\n", clients_p[i]->in_port);
                printf("Received message: %s\n", buffer);
            }
        }
    }
}

void send_to_client(char *message, uint32_t client_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_p[i] != NULL && clients_p[i]->client_id == client_id) {
            TCP_send_message(clients_p[i]->sock_fd, message);
        }
    }
}

static void add_client(connect_manager_t new_client) {
    uint8_t add_flag = 0;
    
    // Find an empty slot in the client array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_p[i] == NULL) {
            clients_p[i] = malloc(sizeof(connect_manager_t));
            clients_p[i]->client_id = new_client.client_id;
            clients_p[i]->sock_fd = new_client.sock_fd;
            clients_p[i]->in_port = new_client.in_port;
            clients_p[i]->in_addr = new_client.in_addr;
            add_flag = 1;
            break;
        }
    }

    printf("\n");
    if (!add_flag) {
        printf("Connection list is full! Cannot add more connections.\n");
    } else {
        printf("New client connected: %s:%d\n", new_client.in_addr, new_client.in_port);
    }
    printf("\n");
}

void remove_client(uint32_t client_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_p[i] != NULL && clients_p[i]->client_id == client_id) {
            TCP_close_connection(clients_p[i]->sock_fd);
            printf("\nFree conection %s on port: %d\n", clients_p[i]->in_addr, clients_p[i]->in_port);
            free(clients_p[i]);
            clients_p[i] = NULL;
            break;
        }
    }
}

void remove_all_client(void) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_p[i] != NULL) {
            TCP_close_connection(clients_p[i]->sock_fd);
            printf("\nFree conection %s on port: %d\n", clients_p[i]->in_addr, clients_p[i]->in_port);
            free(clients_p[i]);
            clients_p[i] = NULL;
            break;
        }
    }
}

void list_all_connections(void) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_p[i] != NULL) {
            printf("Connect ID: %d | IP: %s | Port: %d\n", clients_p[i]->client_id, clients_p[i]->in_addr, clients_p[i]->in_port);
        }
    }
}

void getMyIp(int sock_fd){
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(sock_fd, (struct sockaddr *)&local_addr, &addr_len) == -1) {
        perror("getsockname");
        return;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, ip, sizeof(ip));
    int port = ntohs(local_addr.sin_port);

    printf("Socket bound to IP: %s, Port: %d\n", ip, port);   
}