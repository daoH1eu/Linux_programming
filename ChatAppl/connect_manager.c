#include "connect_manager.h"

// Connection id counter
static uint32_t id_counter = 1;

// Client list
static connect_manager_t *clients_p[MAX_CLIENTS] = {NULL};

// Declare a set of file descriptors
static fd_set read_fds, write_fds;

// Maximum file descriptor
static int max_fd;

// Server socket
static int server_fd;

static inline void update_max_fd(int new_fd)
{
    if (new_fd > max_fd)
        max_fd = new_fd;
}

static void update_select()
{
    // Use select to check activity
    fd_set tmp_read_fds, tmp_write_fds;
    tmp_read_fds = read_fds;
    tmp_write_fds = write_fds;

    // printf("Waiting for activity...\n");
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(max_fd + 1, &tmp_read_fds, &tmp_write_fds, NULL, &timeout) < 0)
    {
        perror("Select error");
    }
}

int create_server_socket(int port)
{
    // Initialize (empty) the set
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    server_fd = TCP_create_socket();

    if (server_fd < 0)
    {
        perror("Failed to create socket");
        return -1;
    }

    if (TCP_bind_socket(server_fd, port) < 0)
    {
        perror("Failed to bind socket");
        return -1;
    }

    if (TCP_listen_for_connection(server_fd) < 0)
    {
        perror("Failed to listen");
        return -1;
    }

    printf("Listening on port: %d\n", port);
    // Monitor the server socket
    FD_SET(server_fd, &read_fds);
    FD_SET(server_fd, &write_fds); 
    update_max_fd(server_fd);
    //update_select();

    return server_fd;
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

void accept_new_connection(void)
{
    int client_port;
    char client_ip[INET_ADDRSTRLEN];
    
    update_select();

    if (FD_ISSET(server_fd, &read_fds))
    {
        int client_fd = TCP_accept_connection(server_fd, &client_port, client_ip);

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
}

void client_handler(void)
{
    update_select();
    // Handle client messages
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL && clients_p[i]->sock_fd > 0)
        {
            if (FD_ISSET(clients_p[i]->sock_fd, &read_fds) && clients_p[i]->sock_fd != server_fd)
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
}

void send_to_client(char *message, uint32_t client_id)
{
    //update_select();
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_p[i] != NULL && clients_p[i]->client_id == client_id)
        {
            if (FD_ISSET(clients_p[i]->sock_fd, &write_fds) && clients_p[i]->sock_fd != server_fd)
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
        // Add new client socket to fds set to monitor
        FD_SET(new_client.sock_fd, &read_fds);
        FD_SET(new_client.sock_fd, &write_fds);
        update_max_fd(new_client.sock_fd);
        //update_select();
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

            FD_CLR(clients_p[i]->sock_fd, &read_fds);
            FD_CLR(clients_p[i]->sock_fd, &write_fds);
            //update_select();

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

            FD_CLR(clients_p[i]->sock_fd, &read_fds);
            FD_CLR(clients_p[i]->sock_fd, &write_fds);

            free(clients_p[i]);
            clients_p[i] = NULL;
        }
    }
    // Initialize (empty) the set
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    //update_select();

    close(server_fd);
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

void get_my_ip(void)
{
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(server_fd, (struct sockaddr *)&local_addr, &addr_len) == -1)
    {
        perror("getsockname");
        return;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &local_addr.sin_addr, ip, sizeof(ip));
    int port = ntohs(local_addr.sin_port);

    printf("Socket bound to IP: %s, Port: %d\n", ip, port);
}
