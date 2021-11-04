#include <sstream>
#include "pixel.h"
#include "../../utils.h"

using namespace records;

Pixel::Pixel(event_no_t event_no, player_number_t player_number,
             coordinates_t coordinates)
    : Event(event_no, event_t::PIXEL), player_number(player_number)
{
    x = coordinates.first;
    y = coordinates.second;
}

network::buffer_t Pixel::get_data() const
{
    network::buffer_t bytes(pixel_head_lengt, '\0');
    char *data = const_cast<char *>(bytes.data());
    size_t off = 0u;

    *reinterpret_cast<player_number_t *>(data + off) = player_number;
    off += sizeof(player_number_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(x);
    off += sizeof(pixel_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(y);
    return bytes;
}

size_t Pixel::get_len() const
{
    return Event::get_len() + pixel_head_lengt;
}

Pixel::Pixel(event_no_t event_no, const char *data,
             size_t length)
    : Event(event_no, event_t::PIXEL)
{
    size_t off = 0u;

    // Check length to avoid unallocated memory access
    if (length != pixel_head_lengt)
    {
        throw invalid_argument("Unexpected data in PIXEL");
    }

    player_number = *reinterpret_cast<const player_number_t *>(data + off);
    off += sizeof(player_number_t);

    x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);

    y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
}

string
Pixel::to_string(const vector<string> &players) const
{
    ostringstream os;
    if (players.size() < player_number)
    {
        throw invalid_argument("Player with given number does not exist");
    }
    os << "PIXEL " << x << " " << y << " " << players[player_number] << "\n";
    return os.str();
}

void Pixel::validate_event(pixel_t width, pixel_t height,
                           size_t players_counter) const
{
    if (x >= width || y >= height)
    {
        throw invalid_argument("PIXEL has invalid coordinates!");
    }

    if (player_number >= players_counter)
    {
        throw invalid_argument("PIXEL for non existent user!");
    }
}
