#ifndef PIXEL_H
#define PIXEL_H

#include "event.h"

using namespace std;

namespace records
{
    constexpr size_t pixel_head_lengt =
        sizeof(player_number_t) + sizeof(pixel_t) * 2;

    class Pixel : public Event
    {
    private:
        /****************************
        *        event params
        ****************************/
        player_number_t player_number;
        pixel_t x;
        pixel_t y;
        network::buffer_t get_data() const  override;

    public:
        Pixel(event_no_t event_no, player_number_t player_number,
              coordinates_t coordinates) ;
        Pixel(event_no_t event_no, const char *data,
              size_t length);

        size_t get_len() const  override;
        string to_string(const vector<string> &players) const override;
        void validate_event(pixel_t width, pixel_t height, size_t players_counter) const override;
    };
} // namespace records

#endif // PIXEL_H
