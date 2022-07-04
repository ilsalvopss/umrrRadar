#include <iostream>
#include "umrrParser.h"

int umrrParser::consume_bytes(const uint8_t *in_buf, const int size) {
    for(int i = 0;i < size; i++){
        auto b = std::byte{in_buf[i]};

        buf.push_back(b);

        switch (state) {
            case IDLE:
                if(head_magic[magic_progress] == b)
                    magic_progress++;
                else if(magic_progress > 0 && head_magic[magic_progress-1] == b)
                    magic_progress = 1;
                else
                    magic_progress = 0;

                if(magic_progress == 4) {
                    state = BUF;
                    magic_progress = 0;
                    checksum = std::byte{0x0};
                    buf.clear();
                }

                break;
            case BUF:
                checksum ^= b;

                if(checksum == std::byte{0x0})
                    last_valid_checksum++;
                else
                    last_valid_checksum = 0;

                if(tail_magic[magic_progress] == b)
                    magic_progress++;
                else if(magic_progress > 0 && tail_magic[magic_progress-1] == b)
                    magic_progress = 1;
                else
                    magic_progress = 0;

                if(magic_progress == 4){
                    state = IDLE;

                    for(int k = 0;k < magic_progress;k++)
                        buf.pop_back();

                    magic_progress = 0;

                    if(last_valid_checksum == magic_progress + 1)
                        parse_buf();
                }
                break;
        }
    }

    return size;
}

inline std::byte pop_front(std::deque<std::byte>& buf){
    std::byte b = buf.front();
    buf.pop_front();
    return b;
}

void umrrParser::parse_buf() {
    buf.pop_back(); // checksum

    while(!buf.empty()){
        const size_t s = buf.size();
        int32_t id;
        int32_t len;

        if(s < 3){
            buf.clear();
            break;
        }

        id = std::to_integer<int32_t>(pop_front(buf)) << 8;
        id |= std::to_integer<int32_t>(pop_front(buf));

        len = std::to_integer<int32_t>(pop_front(buf));

        if(s < len) {
            buf.clear();
            break;
        }

        if(std::find(filters.begin(), filters.end(), id) == filters.end()){
            for(int i = 0;i < len;i++)
                buf.pop_front();

            continue;
        }

        const std::vector<std::byte> can_packet(std::make_move_iterator(buf.begin()), std::make_move_iterator(buf.begin()+len));
        buf.erase(buf.begin(), buf.begin()+len);

        switch (id) {
            case 0x750:
                parse_altimeter(can_packet);
                break;

            default:
                parse_unk(can_packet);
                break;
        }
    }
}

/// parser for can message 0x750
/// altitude + speed + validity bits
void umrrParser::parse_altimeter(const std::vector<std::byte> &m) {
    float altitude = 0.0;
    float speed = 0.0;
    bool valid;

    altitude += std::to_integer<int32_t>(m[7]);
    altitude += std::to_integer<int32_t>(m[6]) << 8;
    altitude += std::to_integer<int32_t>(m[5]) << 16;
    altitude *= 0.01;

    speed += std::to_integer<int32_t>(m[4]);
    speed += std::to_integer<int32_t>(m[3]) << 8;
    speed += std::to_integer<int32_t>(m[2]) << 16;
    speed -= 0x800000;
    speed *= 0.01;

    valid = std::to_integer<uint8_t>(m[1]) & 1;

    parseCallback({altitude,speed, valid});
}

void umrrParser::parse_unk(const std::vector<std::byte> &m){
    /// got unknown message
}