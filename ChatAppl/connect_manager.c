#include "connect_manager.h"

static uint32_t id_counter = 1; // Connection id counter

int create_server_socket(int port)
{
    int sock_fd = TCP_create_socket();

    if (sock_fd < 0)
    {
        perror("Failed to create socket");
        return -1;
    }

    if (TCP_bind_socket(sock_fd, port) < 0)
    {
        perror("Failed to bind socket");
        return -1;
    }

    if (TCP_listen_for_connection(sock_fd) < 0)
    {
        perror("Failed to listen");
        return -1;
    }

    printf("Listening on port: %d\n", port);

    return sock_fd;
}

int create_client_socket(const char *ip, int port)
{
    int sock_fd = TCP_create_socket();

    if (sock_fd < 0)
    {
        perror("Failed to create socket");
        return -1;
    }

    if (TCP_connect(sock_fd, ip, port) < 0)
    {
        perror("Connection failed");
        return -1;
    }

    connect_manager_t client;
    client.client_id = id_counter++;
    client.in_addr = strdup(ip);
    client.in_port = port;
    client.sock_fd = sock_fd;
    add_client(client);

    return sock_fd;
}

void accept_new_connection(int listen_fd)
{
    int client_port;
    char client_ip[INET_ADDRSTRLEN];

    int client_fd = TCP_accept_connection(listen_fd, &client_port, client_ip);

    if (client_fd < 0)
    {
        perror("Failed to accept connection");
        return;
    }

    connect_manager_t client;
    client.client_id = id_counter++;
    client.in_addr = client_ip;
    client.in_port = client_port;
    client.sock_fd = client_fd;
    add_client(client);
}

void client_handler(void)
{
    // Handle client messages
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL && clients_p[i]->sock_fd > 0)
        {
            char buffer[BUFFER_SIZE];
            int bytes_received = TCP_receive_message(clients_p[i]->sock_fd, buffer, BUFFER_SIZE);

            if (bytes_received < 0)
            {
                perror("Failed to receive message");
                return;
            }
            else if (bytes_received == 0)
            {
                printf("\n");
                printf("Connection |%d| disconnected", clients_p[i]->client_id);
                remove_client(clients_p[i]->client_id);
            }
            else
            {
                printf("\n");
                printf("Message received from ID: %d\n", clients_p[i]->client_id);
                printf("IP: %s\n", clients_p[i]->in_addr);
                printf("Port: %d\n", clients_p[i]->in_port);
                printf("Received message: %s\n", buffer);
            }
        }
    }
}

void send_to_client(char *message, uint32_t client_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL && clients_p[i]->client_id == client_id)
        {
            int send_stat = TCP_send_message(clients_p[i]->sock_fd, message);
            if (send_stat == -1)
            {
                perror("Failed to send message");
                return;
            }
            else if (send_stat == -2)
            {
                perror("Message size too large");
                return;
            }
        }
    }
}

static void add_client(connect_manager_t new_client)
{
    uint8_t add_flag = 0;

    // Find an empty slot in the client array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] == NULL)
        {
            clients_p[i] = malloc(sizeof(connect_manager_t));
            clients_p[i]->client_id = new_client.client_id;
            clients_p[i]->sock_fd = new_client.sock_fd;
            clients_p[i]->in_port = new_client.in_port;
            clients_p[i]->in_addr = new_client.in_addr;
            add_flag = 1;
            break;
        }
    }

    if (!add_flag)
    {
        printf("\nConnection list is full! Cannot add more connections.\n");
    }
    else
    {
        printf("\nConnection established: ID: %d | IP: %s | Port: %d\n", new_client.client_id, new_client.in_addr, new_client.in_port);
    }
}

void remove_client(uint32_t client_id)
{
    uint8_t id_found = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL && clients_p[i]->client_id == client_id)
        {
            if (TCP_close_connection(clients_p[i]->sock_fd) < 0)
            {
                printf("Failed to close connection with ID: %d", clients_p[i]->client_id);
                return;
            }

            printf("\nFree conection: ID:  %d | IP: %s | Port: %d\n", clients_p[i]->client_id, clients_p[i]->in_addr, clients_p[i]->in_port);
            free(clients_p[i]);
            clients_p[i] = NULL;
            id_found = 1;
            break;
        }
    }
    if (!id_found)
        printf("Connection with ID: %d not exist\n", client_id);
}

void remove_all_clients(void)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL)
        {
            if (TCP_close_connection(clients_p[i]->sock_fd) < 0)
            {
                printf("Failed to close connection with ID: %d", clients_p[i]->client_id);
                continue;
            }

            printf("\nFree conection: ID: %d | IP: %s | Port: %d\n", clients_p[i]->client_id, clients_p[i]->in_addr, clients_p[i]->in_port);
            free(clients_p[i]);
            clients_p[i] = NULL;
        }
    }
}

void list_all_connections(void)
{
    uint8_t is_empty = 1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL)
        {
            printf("Connect ID: %d | IP: %s | Port: %d\n", clients_p[i]->client_id, clients_p[i]->in_addr, clients_p[i]->in_port);
            is_empty = 0;
        }
    }
    if (is_empty)
        printf("\nNo active connections, list empty!\n");
    ;
}

void getMyIp(int sock_fd)
{
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(sock_fd, (struct sockaddr *)&local_addr, &addr_len) == -1)
    {
        perror("getsockname");
        return;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, ip, sizeof(ip));
    int port = ntohs(local_addr.sin_port);

    printf("Socket bound to IP: %s, Port: %d\n", ip, port);
}