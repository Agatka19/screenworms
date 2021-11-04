#ifndef NEW_GAME_H
#define NEW_GAME_H

#include <set>
#include "event.h"


using namespace std;

namespace records {
    constexpr size_t new_game_head_len = sizeof(pixel_t) * 2;

    class NewGame : public Event {
    private:
        /****************************
        *        event params
        ****************************/
        pixel_t max_x;
        pixel_t max_y;
        set<string> players;
        size_t players_len = 0;

        network::buffer_t get_data() const  override;

    public:
        NewGame(pixel_t max_x, pixel_t max_y, event_no_t event_no) ;
        NewGame(event_no_t event_no, const char *data, size_t length);

        void add_player(const string &player);
        void add_player(string &&player);
        size_t get_len() const  override;
        pixel_t get_width() const ;
        pixel_t get_height() const ;

        set<string>::const_iterator begin() const ;

        set<string>::const_iterator end() const ;

        string to_string(
                const vector<string> &players) const  override;

        void validate_event(pixel_t width, pixel_t height,
                              size_t players_counter) const override;
    };
}

#endif //NEW_GAME_H
