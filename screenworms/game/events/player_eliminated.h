#ifndef PLAYER_ELIMINATED_H
#define PLAYER_ELIMINATED_H

#include "event.h"

using namespace std;

namespace records
{
    class PlayerEliminated : public Event
    {
    private:
        /****************************
        *        player data
        ****************************/
        player_number_t player_number;
        network::buffer_t get_data() const  override;

    public:
        PlayerEliminated(event_no_t event_no, player_number_t player_number) ;
        PlayerEliminated(event_no_t event_no, const char *data, size_t length);

        size_t get_len() const  override;
        string to_string(
            const vector<string> &players) const override;
        void validate_event(pixel_t width, pixel_t height, size_t players_counterer) const override;
    };
    
} // namespace records

#endif //PLAYER_ELIMINATED_H
