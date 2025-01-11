#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // For open() flags
#include <unistd.h> // For read(), write(), close()
#include <sys/types.h>
#include <sys/stat.h> // For file mode constants
#include <string.h>   // For strlen()

int main(int argc, char *argv[]) {
    // Check if the number of arguments is exactly 4 (program name + 3 arguments)
    if (argc != 4) {
        printf("Usage: %s <arg1> <arg2> <arg3> <arg4>\n", argv[0]);
        return 1; // Exit with error if incorrect number of arguments
    }

    /* 
    Process the command line arguments
    ./exe_program filename [r/w] data
    */
    char *arg1 = argv[1];   // filename
    char *arg2 = argv[2];   // action r/w
    char *arg3 = argv[3];   // data for r/w
    int flag = 0;
    int commonFlags = O_CREAT | O_APPEND;
    int mode = S_IRUSR | S_IWUSR;

    if (strlen(arg2) != 1 || (arg2[0] != 'r' && arg2[0] != 'w')) {
        perror("Error: The second argument must be exactly 'r' or 'w'.\n");
        return 1; // Exit with error if invalid second argument
    }

    // Output the arguments
    printf("File name: %s\n", arg1);
    if (*arg2 == 'r'){
        printf("Action: read\n");
        flag = O_RDONLY;
    } else if (*arg2 == 'w'){
        printf("Action: write\n");
        flag = O_WRONLY;
    }
    printf("Data: %s\n\n", arg3);

    // Open file
    int fd = open(arg1, commonFlags | flag, mode); 

    if (fd < 0) {
        perror("Error: Fail to open file\n");
        return 1;
    }

    // Write/read to file
    if (*arg2 == 'r') {
        // Read and print data from the file
        char buffer[1024] = {'\0'};
        ssize_t bytesRead = read(fd, buffer, atoi(arg3));
        printf("Read from file: %s\n", buffer);

        if (bytesRead == -1) {
            perror("Error reading from file\n");
            close(fd); // Close the file descriptor before exiting
            return 1;
        }
    } 
    else if (*arg2 == 'w') {
        // Write data to the file
        ssize_t bytesWritten = write(fd, arg3, strlen(arg3));

        if (bytesWritten == -1) {
            perror("Error writing to file\n");
            close(fd); // Close the file descriptor before exiting
            return 1;
        }
    }

    // Close the file
    close(fd);
    return 0;
}
