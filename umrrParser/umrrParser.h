/*
 * umrrParser library
 * (progetto polimi 2022)
 *
 * Salvatore Passaro
 */

#ifndef MICRORADAR_UMRRPARSER_H
#define MICRORADAR_UMRRPARSER_H

#include <cstddef>
#include <deque>
#include <vector>
#include <istream>
#include <algorithm>

/*
 * defines a data struct containing the fields outputted by the altimeter
 */
struct umrrMessage{
    float altitude = 0.0;
    float vertical_speed = 0.0;
    bool altitude_valid = false;
};

class umrrParser {
    const std::byte head_magic[4] = {std::byte{0xCA}, std::byte{0xCB}, std::byte{0xCC}, std::byte{0xCD}};
    const std::byte tail_magic[4] = {std::byte{0xEA}, std::byte{0xEB}, std::byte{0xEC}, std::byte{0xED}};

    enum parsingState{
        IDLE,
        BUF
    };

    std::deque<std::byte> buf;
    parsingState state = IDLE;
    unsigned short magic_progress = 0;
    std::byte checksum;
    unsigned int last_valid_checksum = 0;
    std::function<void(umrrMessage)> parseCallback;
    std::vector<int> filters;

    void parse_buf();
    void parse_altimeter(const std::vector<std::byte> &);
    void parse_unk(const std::vector<std::byte> &m);

public:
    /*
     * parser constructor
     *
     * pc is a callback which the parser will call on each message successfully parsed
     * messageFilters is a vector containing all the message CAN ids the parser will look for.
     */
    explicit umrrParser(std::function<void(umrrMessage)>& pc, const std::vector<int>& messageFilters = {0x750}){
        filters = messageFilters;
        parseCallback = pc;
        checksum = std::byte{0x0};
    }

    /*
     * consume_bytes should be called every time a new chunk of raw data is received from the altimeter
     */
    int consume_bytes(const uint8_t* b, int size);
};


#endif //MICRORADAR_UMRRPARSER_H