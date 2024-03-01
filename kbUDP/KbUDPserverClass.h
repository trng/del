#pragma once

#include <vector>
#include <sstream>
#include <iostream>

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996)     // disable warning/error for obsolete inet_addr function
#include <winsock2.h>

#include "Stringer.h"
#include "KbUdpPacketHeaders.h"
#include "KbTickerClass.h"


#define BUFLEN 1472


using namespace std;


class KbUDPServerClass {


private:
    SOCKET           server_socket;
    sockaddr_in      server, client = { 0 };
    bool             exitRequested = false;
    uint16_t         port_to_bind_to_;
    MyThreadedClass  myObject;


public:
    /**
    * @brief KbUDPServerClass Constructor.
    * @param port_to_bind_to Port to bind to. No port availability checks.
    */
    KbUDPServerClass(uint16_t port_to_bind_to) {
        port_to_bind_to_ = port_to_bind_to;
        // create a socket
        if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
            printf("Could not create socket: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        printf("Socket created.\n");

        // prepare the sockaddr_in structure
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port_to_bind_to_);

        // bind
        if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
            printf("Bind failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        cout << "Bind done. Port  " << port_to_bind_to_ << "  is currently in listening mode.\n\n";
    }

    ~KbUDPServerClass() {
        closesocket(server_socket);
        WSACleanup();
        myObject.stopThreadFunction();
    }

    /**
     * @brief Basic UDP listener (just responds with incoming message).
     */
    void startBasicUdpListener() {
        while (!exitRequested) {
            printf("Waiting for data...");
            fflush(stdout);
            char message[BUFLEN] = {};

            // try to receive some data, this is a blocking call
            int message_len;
            int slen = sizeof(sockaddr_in);
            if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR) {
                printf("recvfrom() failed with error code: %d", WSAGetLastError());
                exit(0);
            }

            // print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

            printf("Unparsed incoming data: %s\n", message);



            //cout << "Enter response (exit to stop server process): ";
            //cin.getline(message, BUFLEN);
            char message_reponse[BUFLEN] = {};
            snprintf(message_reponse, BUFLEN - 1, "Your message: %s", message);

            // reply to the client with the same data
            if (sendto(server_socket, message_reponse, int(strlen(message_reponse)), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                printf("sendto() failed with error code: %d", WSAGetLastError());
                exit(EXIT_FAILURE);
            }

            if (strcmp(message, "exit") == 0) {
                std::cout << "Exiting server...\n";
                exitRequested = true;
                break;
            }
        }
    }

    
    /** 
     * @brief BasicUdpListener plus charcode to string conversion for all chars
     */
    void startCharCodesUdpListener() {
        while (!exitRequested) {
            printf("Waiting for data...");
            fflush(stdout);
            char message[BUFLEN] = {};

            // try to receive some data, this is a blocking call
            int message_len;
            int slen = sizeof(sockaddr_in);
            if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR) {
                printf("recvfrom() failed with error code: %d", WSAGetLastError());
                exit(0);
            }

            // print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

            string s = "";
            
            for (int i = 0; i < message_len; i++) {
                s += to_string((uint8_t)message[i]) + " ";
            }


            printf("incoming data as char codes: %s\n", s.data());



            //cout << "Enter response (exit to stop server process): ";
            //cin.getline(message, BUFLEN);
            char message_reponse[BUFLEN] = {};
            snprintf(message_reponse, BUFLEN - 1, "Your message: %s", s.data());

            // reply to the client with the same data
            if (sendto(server_socket, message_reponse, int(strlen(message_reponse)), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                printf("sendto() failed with error code: %d", WSAGetLastError());
                exit(EXIT_FAILURE);
            }

            if (strcmp(message, "exit") == 0) {
                std::cout << "Exiting server...\n";
                exitRequested = true;
                break;
            }
        }
    }


    void startUdpListenerForVmixTiming() {
        while (!exitRequested) {
            printf("Waiting for data...\n");
            fflush(stdout);
            char message[BUFLEN] = {};

            // try to receive some data, this is a blocking call
            int message_len;
            int slen = sizeof(sockaddr_in);
            if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR) {
                printf("\nrecvfrom() failed with error code: %d", WSAGetLastError());
                return;
            }

            // print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            // if (message_len>2 && message[0]==73 && message[1]==73) {
            if (message_len > 2 && *(uint16_t*)message == 0x4949 && message[2] > 0 && message[2] < 8) { // only 7 commands availible for now
                switch (message[2]) {
                    case 0: zeroHandler(message, message_len);  break;
                    case 1: startTimer(message, message_len);   break;
                    case 2: break;
                    case 3: break;
                    case 4: break;
                    case 5: break;
                    case 6: getTimerHandler(message, message_len);  break;
                    case 7: subscribeToTimerHandler(message, message_len); break;
                    default: break;
                }
            }
            else {
                // Wrong message format
                cout <<  kb_response_codes_str.at(wrong_message_format) << "\n";
                
                // reply to the client with error message
                GeneralResponseUdpPacketHeader err_pkt_hdr = { .response_code = wrong_message_format };
                if (sendto(server_socket, (const char *) & err_pkt_hdr, sizeof(err_pkt_hdr), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                    printf("sendto() failed with error code: %d", WSAGetLastError());
                    // exit(EXIT_FAILURE);
                }
            }

            //if (strcmp(message, "exit") == 0) {
            //    std::cout << "Exiting server...\n";
            //    exitRequested = true;
            //    break;
            // }
        }
    }


    void  zeroHandler(const char* message, int message_len) {};


    void startTimer(const char* message, int message_len) {
        cout << "\n\nstartTimer\n\n";
        // First three bytes already checked (73,73,1)
        char resopnse_status = 1;
        // parse starttimer header
        StartTimerRequestUdpPacketHeader * pktHdr;
        pktHdr = (StartTimerRequestUdpPacketHeader*) message;
        if (pktHdr->timer_no > 0 && pktHdr->seconds < 60) {

            resopnse_status = 0;
            printf("\ncommand %d   timer_no %d     start_mins %d     start_secs %d\n", pktHdr->command, pktHdr->timer_no, pktHdr->minutes, pktHdr->seconds);

            //KbTickerClass kbtc;
            // auto t = kbtc.start();
            if (myObject.isThreadActive()) {
                myObject.stopThreadFunction();
            }
            myObject.startThread(pktHdr->minutes, pktHdr->seconds);
        }
        else {
            // everyting is bad
        }


        // reply to the client whith one-byte response status
        if (sendto(server_socket, &resopnse_status, sizeof(resopnse_status), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            // exit(EXIT_FAILURE);
        }
    }


    void getTimerHandler(const char* message, int message_len) {
        cout << "\n\ngetTimer handler\n\n";
        // First three bytes already checked (73,73,6)

        // parse starttimer header
        GetTimerRequestUdpPacketHeader  * request_pkt_hdr  = (GetTimerRequestUdpPacketHeader*)message;
        GetTimerResponseUdpPacketHeader response_pkt_hdr;
        if (request_pkt_hdr->timer_no = 0) {
            cout << "Timer number cannot be 0\n";
            response_pkt_hdr = { .response_code = 1, .timer_no = 0, .minutes = 0, .seconds = 0 };
        }
        else {
            printf("\ncommand %d   timer_no %d     \n", request_pkt_hdr->command, request_pkt_hdr->timer_no);
            // reply to the client with time
            response_pkt_hdr = { .timer_no = 1, .minutes = 34, .seconds = 56 };
        }
        if (sendto(server_socket, (char*)&response_pkt_hdr, sizeof(response_pkt_hdr), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR) {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            // exit(EXIT_FAILURE);
        }
    };


    void subscribeToTimerHandler(const char* message, const int message_len) {
        cout << "\n\nsubscribeToTimerHandler\n\n";
        
        /**
        *   First three bytes already checked (73, 73, command-code)
        */ 

        // parse starttimer header
        SubscribeToTimerRequestUdpPacketHeader * pktHdr  = (SubscribeToTimerRequestUdpPacketHeader*)message;
        if (pktHdr->timer_no == 0) {
            // error
        }
        else {
            printf("\ncommand %d   timer_no %d     \n", pktHdr->command, pktHdr->timer_no);
            myObject.socketInit(pktHdr->ipv4_addr, pktHdr->udp_port);
           
        }
        GeneralResponseUdpPacketHeader response_pkt_hdr;
        if (sendto(server_socket, (char*)&response_pkt_hdr, sizeof(response_pkt_hdr), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
            printf("sendto() failed with error code: %d", WSAGetLastError());
    }

}; // End of KbUDPserverClass