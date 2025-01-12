#include "TCP_socket.h"

int TCP_create_socket() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    int ret_val = 0;

    if (sock_fd < 0) {
        // perror("Failed to create socket");
        ret_val = -1;
    }

    if (ret_val == 0)
        ret_val = sock_fd;

    return ret_val;
}

int TCP_bind_socket(int sock_fd, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int ret_val = 0;

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        // perror("Invalid address or address not supported");
        ret_val = -1;
    }

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // perror("Failed to bind socket");
        ret_val = -1;
    }
    return ret_val;
}

int TCP_listen_for_connection(int sock_fd) {
    int ret_val = 0;
    if (listen(sock_fd, 1) < 0) {
        // perror("Failed to listen");
        ret_val = -1;
    }
    return ret_val;
}

int TCP_accept_connection(int sock_fd, int *port, char *client_ip) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sockfd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);
    int ret_val = 0;

    if (client_sockfd < 0) {
        // perror("Failed to accept connection");
        ret_val = -1;
    } else {
        // Retrieve the client's IP and port
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) == NULL) {
            // perror("Failed to convert client IP address");
            close(client_sockfd);
            ret_val = -1;
        }
        *port = ntohs(client_addr.sin_port);
    }

    if (ret_val == 0)
        ret_val = client_sockfd;

    return ret_val;
}

int TCP_connect(int sock_fd, const char *ip, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int ret_val = 0;
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        // perror("Invalid address or address not supported");
        ret_val = -1;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // perror("Connection failed");
        ret_val = -1;
    }
    return ret_val;
}

int TCP_send_message(int sock_fd, const char *message) {
    int ret_val = 0;
    if (send(sock_fd, message, strlen(message), 0) < 0) {
        // perror("Failed to send message");
        ret_val = -1;
    }
    return ret_val;
}

int TCP_receive_message(int sock_fd, char *buffer, uint32_t size) {
    ssize_t bytes_received = recv(sock_fd, buffer, size, 0);
    int ret_val = 0;

    if (bytes_received < 0) {
        // perror("Failed to receive message");
        ret_val = -1;
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the message
    }

    if (ret_val == 0)
        ret_val = bytes_received;

    return ret_val;
}

int TCP_close_connection(int sock_fd) {
    int ret_val = 0;
    if (close(sock_fd) < 0) {
        // perror("Failed to close connection");
        ret_val = -1;
    }
    return ret_val;
}
