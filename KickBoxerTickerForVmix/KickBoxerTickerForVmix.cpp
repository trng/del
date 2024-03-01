#pragma warning(disable:4996)     // disable warning/error for obsolete inet_addr function (winsock2)
#pragma comment(lib,"ws2_32.lib") // Winsock Library
#include <winsock2.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <windows.h>
#include "KbUDPserverClass.h"
// #include "KbTickerClass.h"


using namespace std;


WSADATA wsa_data; // winsock
uint16_t udp_port_to_bind_to = 8888;


int main(int argc, char* argv[])
{
    // Console window set-up (title, disable mouse select, disable manual scroll - otherwise code execution in the console is suspended)
    {
        system("title KickBoxer3000 ticker for vMix.     Build date: "  __DATE__ ".     Build time: " __TIME__ ".");

        // disable mouse selection inside the window (due to code execution will be suspended in this console during mouse selection)
        DWORD prev_mode = 0;
        bool res = GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prev_mode);
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));

        // disable window resize for disabling scroll (manual scroll suspends code execution in this console)
        HWND consoleWindow = GetConsoleWindow();
        SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    }


    // command line args processing
    {
        std::cout << "KickBoxer3000 ticker for vMix (udp listener).\n\n";
        if (argc != 2) {
            cout << "Usage                     :  kbUDPclient port (to bind to)\n";
            cout << "Example                   :  kbUDPclient " << udp_port_to_bind_to << "\n";
            cout << "Without params (defaults) :  kbUDPclient " << udp_port_to_bind_to << "\n";
        }
        else {
            uint16_t argv_port;
            try {
                argv_port = stoi(argv[1]);
                udp_port_to_bind_to = argv_port;
            }
            catch (...) {
                cout << "Wrong port number. Exiting...\n"; return 0;
            }
        }
        cout << "UDP port to bind to   :   " << udp_port_to_bind_to << "\n\n";
    }


    // initialise winsock2
    {
        printf("Initialising Winsock... ");
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            printf("Failed. Error Code: %d", WSAGetLastError());
            exit(0);
        }
        printf("Initialised.\n");
    }


	KbUDPServerClass kbUdpServer(udp_port_to_bind_to);
    kbUdpServer.startUdpListenerForVmixTiming();
    

    WSACleanup();

}
