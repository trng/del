#include <stdlib.h>
#include "KbUDPserverClass.h"

#pragma warning(disable:4996)     // disable warning/error for obsolete inet_addr function
#pragma comment(lib,"ws2_32.lib") // Winsock Library
#include <winsock2.h>


WSADATA wsa_data;

int main() {
    system("title startCharCodesUdpListener.     Build date: "  __DATE__ ".     Build time: " __TIME__ ".");

    // initialise winsock2 
    printf("Initialising Winsock... ");
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(0);
    }
    printf("Initialised.\n");

    cout <<"port to bind to: "<< 9999 << "\n";
    KbUDPServerClass kbUdpServer(9999);
    kbUdpServer.startCharCodesUdpListener();


    WSACleanup();
}