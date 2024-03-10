#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include "KbUdpPacketHeaders.h"
#include "KbTickerReceivers.h"

//#include <mutex>
//#include <condition_variable>
#include <atomic>
#include <format>
#include <iostream>

using namespace std;


class KbTickerThreadedClass {


private:
    std::thread myThread;
    bool stopThreadFlag = true;
    SendTimerUdpPacketHeader udp_pkt_buf;
    std::chrono::time_point<std::chrono::system_clock> start_time_;
    std::chrono::time_point<std::chrono::system_clock> end_time_;
    std::chrono::time_point<std::chrono::system_clock> next_time_;
    bool* do_not_show_current_time_;

public:
    uint8_t           ticker_no    = 0;
    uint8_t           start_mins_  = 0;
    uint8_t           start_secs_  = 0;
    uint8_t           end_mins_    = 0;
    uint8_t           end_secs_    = 0;
    uint8_t           last_sent_mm = 0; // atomic_uint8_t?
    uint8_t           last_sent_ss = 0; // atomic_uint8_t?
    KbTickerReceivers udp_tcp_receivers;


    KbTickerThreadedClass(bool* ptr) : do_not_show_current_time_(ptr) {}

    void startThread(uint8_t start_mins, uint8_t start_secs, uint8_t end_mins, uint8_t end_secs) {
        start_mins_ = start_mins;
        start_secs_ = start_secs;
        end_mins_   = end_mins;
        end_secs_   = end_secs;
        stopThreadFlag = false;  // Reset the flag
        myThread = std::thread(&KbTickerThreadedClass::threadFunction, this);
    }

    void stopThreadFunction() {
        stopThreadFlag = true;

        if (myThread.joinable()) {
            myThread.join();
        }
    }

    void threadFunction() {
        start_time_  = chrono::system_clock::now(); // chrono::floor<chrono::seconds>()
        next_time_   = start_time_;
        end_time_    = start_time_;
        start_time_ -= 1s * (60 * start_mins_);
        start_time_ -= 1s * start_secs_;
        end_time_   += 1s * (60 * end_mins_);
        end_time_   += 1s * end_secs_;
        chrono::time_point<chrono::system_clock> breakpoint_time_;

        next_time_ += 1s;
        while (!stopThreadFlag) {
            std::this_thread::sleep_until(next_time_);
            if (!stopThreadFlag) {
                if ( next_time_ <= end_time_ ) {
                    // not end time
                    per_second_updates();
                    breakpoint_time_ = chrono::system_clock::now(); // chrono::floor<chrono::seconds>()
                    next_time_ += 1s;
                    if (next_time_ <= breakpoint_time_)
                        next_time_ = breakpoint_time_ + 1s;
                    
                } else {
                    // end time
                    // 1. send udp packet (inform controller)
                    // 2. stop http-get requests to vmix
                    // 3. stop udp sends to controller
                    stopThreadFlag = true;
                }
            }
        }
    }

    bool isThreadActive() const {
        return myThread.joinable();
    }

    void per_second_updates() {
        auto now = chrono::system_clock::now(); // chrono::floor<chrono::seconds>()
        chrono::hh_mm_ss hms { now - floor<chrono::days>(now)};
        
        // const std::time_t t_c = std::chrono::system_clock::to_time_t(next_time_ - start_time_);
        chrono::hh_mm_ss timenow{ chrono::floor<chrono::seconds>(next_time_ - start_time_) };
        uint8_t mm = timenow.hours().count()*60 + timenow.minutes().count();
        uint8_t ss = timenow.seconds().count();
        last_sent_mm = mm;
        last_sent_ss = ss;
        if ( ! *do_not_show_current_time_ )
            cout << "\rNow       " << "\033[" << 9 + ticker_no * 8 << "G" << to_string(mm) << ':' << to_string(ss) << "   ";
        for (const auto& obj : udp_tcp_receivers.ticker_receivers)
            obj->sendPacketMMSS(mm, ss, ticker_no);
    }

};
