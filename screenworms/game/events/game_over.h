#ifndef GAME_OVER_H
#define GAME_OVER_H

#include "event.h"

using namespace std;

namespace records {
    class GameOver : public Event {
    private:
        network::buffer_t get_data() const  override;

    public:
        GameOver(event_no_t event_no) ;
        GameOver(event_no_t event_no, size_t length);

        string to_string(
                const vector<string> &players) const  override;
        void validate_event(pixel_t width, pixel_t height,
                              size_t players_counterer) const override;
    };
}

#endif //GAME_OVER_H