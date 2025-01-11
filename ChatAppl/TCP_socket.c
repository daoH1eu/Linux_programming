#include "TCP_socket.h"

int TCP_create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return -1;
    }
    return sockfd;
}

int TCP_bind_socket(int sockfd, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        return -1;
    }
    return 0;
}

int TCP_listen_for_connection(int sockfd) {
    if (listen(sockfd, 1) < 0) {
        perror("Failed to listen");
        return -1;
    }
    return 0;
}

int TCP_accept_connection(int sockfd, int *port, char *client_ip) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

    if (client_sockfd < 0) {
        perror("Failed to accept connection");
    } else {
        // Retrieve the client's IP and port
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) == NULL) {
            perror("Failed to convert client IP address");
            close(client_sockfd);
            return -1;
        }
        *port = ntohs(client_addr.sin_port);
    }

    return client_sockfd;
}

int TCP_connect(int sockfd, const char *ip, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    return 0;
}

void TCP_send_message(int sockfd, const char *message) {
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Failed to send message");
    }
}

int TCP_receive_message(int sockfd, char *buffer, uint32_t size) {
    ssize_t bytes_received = recv(sockfd, buffer, size, 0);
    
    if (bytes_received < 0) {
        perror("Failed to receive message");
    } else if (bytes_received == 0) {
        printf("Client disconnected.\n");
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the message
    }

    return bytes_received;
}

void TCP_close_connection(int sockfd) {
    if (close(sockfd) < 0) {
        perror("Failed to close connection");
    }
}
