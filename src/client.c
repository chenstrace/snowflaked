#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <netdb.h>
#include <getopt.h>
#include <unistd.h>

void send_command(int sd, char *command);

int main(int argc, char **argv) {
    char *ipaddress = "127.0.0.1";
    int port = 8008;

    int c;
    while (1) {
        static struct option long_options[] = {
            {"ip", required_argument, 0, 'i'},
            {"port", required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "i:p:", long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 0:
                if (long_options[option_index].flag != 0) {
                    break;
                }
                printf("option %s", long_options[option_index].name);
                if (optarg) {
                    printf(" with arg %s", optarg);
                }
                printf("\n");
                break;
            case 'i':
                ipaddress = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                /* getopt_long already printed an error message. */
                break;
            default:
                abort();
        }
    }

    int action = -1;

    if (argc - optind == 0) {
        printf("Command not provided.\n");
        printf("usage: client [--ip=] [--port=] <command> [... command arguments]\n");
        exit(1);
    }

    if (strcmp(argv[optind], "get") == 0) {
        if (argc - optind != 1) {
            printf("The 'get' command requires 0 command parameters.\n");
            printf("usage: client [--ip=] [--port=] next\n");
            exit(1);
        }
        action = 1;
    }

    if (strcmp(argv[optind], "info") == 0) {
        if (argc - optind != 1) {
            printf("The 'info' command requires 0 command parameters.\n");
            printf("usage: client [--ip=] [--port=] info\n");
            exit(1);
        }
        action = 2;
    }

    if (action == 0) {
        printf("Invalid command given, should be either update, next, peek or info.\n");
        printf("usage: client [--ip=] [--port=] <command> [... command arguments]\n");
        exit(1);
    }

    struct hostent *hp;
    struct sockaddr_in pin;
    int sd;

    if ((hp = gethostbyname(ipaddress)) == 0) {
        perror("gethostbyname");
        exit(1);
    }

    memset(&pin, 0, sizeof (pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *) (hp->h_addr))->s_addr;
    pin.sin_port = htons(port);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(sd, (struct sockaddr *) &pin, sizeof (pin)) == -1) {
        perror("connect");
        exit(1);
    }

    switch (action) {
        case 1:
            send_command(sd, "GET\r\n");
            break;
        case 2:
            send_command(sd, "INFO\r\n");
            break;
        default:
            break;
    }

    close(sd);

    return 0;
}

void send_command(int sd, char *command) {
    if (send(sd, command, strlen(command), 0) == -1) {
        perror("send");
        exit(1);
    }
    char buf[1024];
    int numbytes;
    if ((numbytes = recv(sd, buf, 1024 - 1, 0)) == -1) {
        perror("recv()");
        exit(1);
    }

    char *resp = NULL;
    int buf_len;

    switch (buf[0]) {
        case '-':
            printf("Received Error\n");
            break;
        case '+':
        case ':':
            buf_len = strlen(buf) - 3;
            if (buf_len >= 2) {
                resp = malloc(1 + buf_len);
                memcpy(resp, buf + 1, buf_len);
                printf("%s\n", resp);
                free(resp);
            } else {
                printf("protocol error \n");
            }
            break;
        default:
            printf("%s\n", buf);
            break;
    }
}
