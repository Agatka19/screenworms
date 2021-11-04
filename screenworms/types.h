#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <chrono>

using namespace std;

namespace records
{
    /****************************
     *         identifiers
     ****************************/
    using pixel_t = uint32_t;
    using rounds_t = uint32_t;
    using turn_speed_t = uint32_t;
    using rand_t = uint32_t;

    using game_t = uint32_t;
    using session_t = uint64_t;
    using event_no_t = uint32_t;
    using len_t = uint32_t;
    using crc32_t = uint32_t;

    using player_number_t = uint32_t;
    using coordinates_t = pair<pixel_t, pixel_t>;

    /****************************
     *           enums
     ****************************/
    enum class IpVersion
    {
        None,
        IPv4,
        IPv6,
    };

    enum direction_t : int
    {
        STRAIGHT = 0,
        RIGHT = 1,
        LEFT = 2,
    };

    enum event_t : int
    {
        NEW_GAME = 0,
        PIXEL = 1,
        PLAYER_ELIMINATED = 2,
        GAME_OVER = 3
    };

    /****************************
     *         constants
     ****************************/
    static const size_t mess_header_length = sizeof(game_t);
    static const size_t max_message_length = 548;
    static const size_t max_mess_data_length = max_message_length - mess_header_length;
} // namespace records


namespace network
{
    /****************************
     *         identifiers
     ****************************/
    using port_t = uint32_t;
    using buffer_t = string;

    /****************************
     *         constants
     ****************************/
    static const port_t min_port = 0;
    static const port_t max_port = 65530;
    static const size_t max_message_length = 548;
} // namespace network


using ms = chrono::milliseconds;
using clock_time = chrono::system_clock;

#endif //TYPES_H