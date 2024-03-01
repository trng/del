#pragma once

#include  <cstdint>
#include <map>

#define  kbBUFLEN    1472         // max length of answer


enum kb_response_codes_enum : uint8_t {
    everything_good             = 0,
    everything_bad              = 1,
    wrong_message_format        = 2,
    wrong_number_of_args        = 3,
    everything_good_and_payload = 200
};

const std::map <kb_response_codes_enum, std::string> kb_response_codes_str = {
    {everything_good,             "Everithing good"},
    {everything_bad,              "Everything bad"},
    {wrong_message_format,        "Wrong message format"},
    {wrong_number_of_args,        "Wrong number of args for given command"},
    {everything_good_and_payload, "everithing good and response has payload after return code"}
};


struct udp_message_T {
    char message[kbBUFLEN];
    int  message_len;
};


/**
 {IFF1, IFF2, response_code} 
 By default response_code = 0 (everything_good)
 */
struct GeneralResponseUdpPacketHeader {
    uint8_t IFF1 = 37;
    uint8_t IFF2 = 37;
    uint8_t response_code = everything_good;
};



#define StartTimerRequestUdpPacketHeaderSignificantFieldsCount 4
struct StartTimerRequestUdpPacketHeader
{
    uint8_t IFF1 = 73;
    uint8_t IFF2 = 73;
    uint8_t command = 1;
    uint8_t timer_no = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
};




#define GetTimerRequestUdpPacketHeaderSignificantFieldsCount 2
struct GetTimerRequestUdpPacketHeader
{
    uint8_t IFF1 = 73;
    uint8_t IFF2 = 73;
    uint8_t command = 6;
    uint8_t timer_no = 0;
};

struct GetTimerResponseUdpPacketHeader
{
    uint8_t IFF1 = 37;
    uint8_t IFF2 = 37;
    uint8_t response_code = 2;
    uint8_t timer_no = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
};




#define SubscribeToTimerRequestUdpPacketHeaderSignificantFieldsCount 4
struct SubscribeToTimerRequestUdpPacketHeader
{
    uint8_t  IFF1 = 73;
    uint8_t  IFF2 = 73;
    uint8_t  command = 7;
    uint8_t  timer_no = 0;
    uint8_t  ipv4_addr[4] = {0,0,0,0};
    uint16_t udp_port = 0;
};



struct SendTimerEverySecondUdpPacketHeader
{
    uint8_t IFF1 = 37;
    uint8_t IFF2 = 37;
    uint8_t response_code = 2;
    uint8_t timer_no = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
};
