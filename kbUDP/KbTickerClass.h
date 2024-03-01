#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include "KbUdpPacketHeaders.h"

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996)     // disable warning/error for deprecated inet_addr function
#include <winsock2.h>

//#include <mutex>
//#include <condition_variable>
#include <atomic>
#include <format>
#include <iostream>

using namespace std;


class MyThreadedClass {
public:
    MyThreadedClass() : stopThreadFlag(false) {}

    uint8_t start_mins_ = 0;
    uint8_t start_secs_ = 0;
    void startThread(uint8_t start_mins, uint8_t start_secs) {
        start_mins_ = start_mins;
        start_secs_ = start_secs;
        stopThreadFlag = false;  // Reset the flag
        myThread = std::thread(&MyThreadedClass::threadFunction, this);
    }

    void stopThreadFunction() {
        stopThreadFlag = true;

        if (myThread.joinable()) {
            myThread.join();
        }
    }

    void threadFunction() {
        start_time_ = std::chrono::system_clock::now();
        next_time_ = start_time_;
        start_time_ -= 1s * (60 * start_mins_);
        start_time_ -= 1s * start_secs_;
        std::chrono::time_point<std::chrono::system_clock> breakpoint_time_;

        next_time_ += 1s;
        while (!stopThreadFlag) {
            std::this_thread::sleep_until(next_time_);
            if (!stopThreadFlag) {
                breakpoint_time_ = std::chrono::system_clock::now();
                next_time_ += 1s;
                if (next_time_ <= breakpoint_time_) 
                    next_time_ = breakpoint_time_ + 1s;
                myFunction();
            }
        }
    }

    bool isThreadActive() const {
        return myThread.joinable();
    }

    void myFunction() {
        auto now = std::chrono::system_clock::now();
        chrono::hh_mm_ss hms { now - floor<chrono::days>(now)};
        std::cout << "Function in a separate thread. The system clock is currently at " << now << '\n';
        if (is_udp_sender_active) {
            // const std::time_t t_c = std::chrono::system_clock::to_time_t(next_time_ - start_time_);
            chrono::hh_mm_ss timenow{ chrono::floor<chrono::seconds>(next_time_ - start_time_) };
            uint8_t mm = timenow.hours().count()*60 + timenow.minutes().count();
            uint8_t ss = timenow.seconds().count();
            SendTimerEverySecondUdpPacketHeader udp_pkt_buf = { .minutes = (uint8_t)mm, .seconds = (uint8_t)ss };
            if (sendto(client_socket, (char*)&udp_pkt_buf, sizeof(udp_pkt_buf), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                printf("sendto() failed with error code: %d", WSAGetLastError());
                return; //  exit(EXIT_FAILURE);
            }
            // httpGet( to_string(timenow.minutes().count()) + ":" + to_string(timenow.seconds().count()) );
            httpGet( std::format("{:#02}:{:#02}", mm, ss) );
        }


    }

    void socketInit(uint8_t ipaddr[4], uint16_t PORT) {
        // create socket
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
            printf("socket() failed with error code: %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        // Set timeout for recvfrom()
        DWORD recvfrom_timeout = 1 * 2000;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&recvfrom_timeout, sizeof(recvfrom_timeout));

        // setup address structure
        memset((char*)&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        char ipaddr_buf[99];
        int strlen = snprintf(ipaddr_buf, sizeof(ipaddr_buf), "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        // ipaddr conversion error processing - not implmented yet if (strlen>0) {} else {       // error   }
        server.sin_addr.S_un.S_addr = inet_addr(ipaddr_buf);
        is_udp_sender_active = true;
    }


    void httpGet(string timenow) {
        string url = "/API/?Function=SetText&Input=0437300f-2a44-4089-bf76-bb19a74e9bc1&SelectedName=Message&Value=" + timenow; // http://

        SOCKET Socket;
        SOCKADDR_IN SockAddr;
        int lineCount = 0;
        int rowCount = 0;
        struct hostent* host;
        string get_http;

        get_http = "GET " + url + " HTTP/1.1\r\nHost: 127.0.0.1:8088\r\nConnection: close\r\n\r\n";

        Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        host = gethostbyname(url.c_str());

        SockAddr.sin_port = htons(8088);
        SockAddr.sin_family = AF_INET;
        SockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

        if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
            cout << "\n\nERROR: Could not connect to vmix api\n\n";
            // system("pause");
            //return 1;
        }
        else {
            send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);
        }
        

        /* int nDataLength;
        string website_HTML;
        char buffer[100000];
        while ((nDataLength = recv(Socket, buffer, 100000, 0)) > 0) {
            int i = 0;
            while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {

                website_HTML += buffer[i];
                i += 1;
            }
        }*/

        closesocket(Socket);
    }


private:
    std::thread myThread;
    bool stopThreadFlag;
    SendTimerEverySecondUdpPacketHeader udp_pkt_buf;
    SOCKET client_socket = {};
    sockaddr_in server = {};
    std::chrono::time_point<std::chrono::system_clock> start_time_;
    std::chrono::time_point<std::chrono::system_clock> next_time_;
    bool is_udp_sender_active = false;

};

//int main() {
//    MyThreadedClass myObject;
//    myObject.startThread();
//    std::this_thread::sleep_for(std::chrono::seconds(100500));
//    myObject.stopThreadFunction();

//    return 0;
//}
