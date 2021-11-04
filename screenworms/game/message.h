#ifndef MESSAGE_H
#define MESSAGE_H

#include "../types.h"
#include <memory>
#include "events/event.h"
#include <string>
#include <vector>

using namespace std;
using namespace network;

namespace records
{
    constexpr size_t client_mess_header_length = sizeof(session_t) + sizeof(direction_t) + sizeof(event_no_t);
    const size_t data_length = sizeof(session_t) + sizeof(direction_t) + sizeof(event_no_t);

    class ClientMessage
    {
    private:
        /****************************
        *           params
        ****************************/
        session_t session_id;
        direction_t turn_direction;
        event_no_t next_expected_event_no;
        string player_name;

    public:
        ClientMessage(session_t session_id,
                      direction_t turn_direction, event_no_t next_expected_event_no, const string &player_name);
        ClientMessage(session_t session_id,
                      direction_t turn_direction, event_no_t next_expected_event_no, string &&player_name = "");
        ClientMessage(const buffer_t &bytes);

        buffer_t to_bytes() const ;
        session_t get_session() const ;
        direction_t get_turn_direction() const ;
        event_no_t get_next_expected_event_no() const ;
        const string &get_player_name() const ;
    };

    class ServerMessage // server -> client
    {
    private:
        /****************************
        *           params
        ****************************/
        game_t game_id;
        size_t message_len = 0;
        vector<shared_ptr<Event>> events;

    public:
        ServerMessage(game_t game_id) ;
        ServerMessage(const buffer_t &bytes);
        void add_event(shared_ptr<Event> event);
        buffer_t to_bytes() const ;
        game_t get_game_id() const ;

        vector<shared_ptr<Event>>::iterator begin();

        vector<shared_ptr<Event>>::iterator end();
    };

} // namespace records

#endif //MESSAGE_H
