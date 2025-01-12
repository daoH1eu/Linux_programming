#include "TCP_socket.h"
#include "connect_manager.h"
#include <pthread.h>

#define MAX_INPUT_SIZE 100

void display_menu() {
    printf("\n*** Chat App Demo ***\n\n");
    printf("User commands:\n");
    printf("1. help                                 : Show help\n");
    printf("2. myIp                                 : Display the server's IP address\n");
    printf("3. myPort                               : Display the server's port\n");
    printf("4. connect <ip> <port>                  : Connect to a remote server\n");
    printf("5. list                                 : Display the list of active connections\n");
    printf("6. terminate <connection id>            : Terminate a specific connection\n");
    printf("7. send <connection id> <message>       : Send a message to a specific connection\n");
    printf("8. exit                                 : Exit the application\n");
    printf("\nEnter a command\n");
}

void *accept_connection_thread(void *arg) {
    int server_fd = *((int *)arg);
    while(1) {
        accept_new_connection(server_fd);
    }
}

void *client_handler_thread(void *arg) {
    while (1) {
        client_handler();
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Exit with error due to incorrect number of arguments\n");
        return 1; 
    }

    int port = atoi(argv[1]);
    int server_fd = create_server_socket(port);
    
    pthread_t accept_thread, handler_thread;
    // Create the accept connection thread
    pthread_create(&accept_thread, NULL, accept_connection_thread, &server_fd);
    // Create the client handler thread
    pthread_create(&handler_thread, NULL, client_handler_thread, NULL);

    char input[MAX_INPUT_SIZE];
    display_menu();

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Parse the input
        char *command = strtok(input, " ");
        if (command == NULL) {
            printf("Invalid command. Type 'help' for a list of commands.\n");
            continue;
        }

        if (strcmp(command, "help") == 0) {
            display_menu();
        } else if (strcmp(command, "myIp") == 0 || strcmp(command, "myPort") == 0) {
            getMyIp(server_fd);
        } else if (strcmp(command, "connect") == 0) {
            char *ip = strtok(NULL, " ");
            char *port = strtok(NULL, " ");
            if (ip == NULL || port == NULL) {
                printf("Usage: connect <ip> <port>\n");
            } else {
                create_client_socket(ip, atoi(port));
            }
        } else if (strcmp(command, "list") == 0) {
            list_all_connections();
        } else if (strcmp(command, "terminate") == 0) {
            char *connection_id = strtok(NULL, " ");
            if (connection_id == NULL) {
                printf("Usage: terminate <connection id>\n");
            } else {
                remove_client(atoi(connection_id));
            }
        } else if (strcmp(command, "send") == 0) {
            char *connection_id = strtok(NULL, " ");
            char *message = strtok(NULL, "");
            if (connection_id == NULL || message == NULL) {
                printf("Usage: send <connection id> <message>\n");
            } else {
                // handle_send(connection_id, message);
                send_to_client(message, atoi(connection_id));
            }
        } else if (strcmp(command, "exit") == 0) {
            // handle_exit();
            remove_all_clients();
            close(server_fd);
            exit(0);
        } else {
            printf("Invalid command. Type 'help' for a list of commands.\n");
        }
    }

    return 0;
}
