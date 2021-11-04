#include <stdexcept>
#include "message.h"
#include "events/create.h"

using namespace records;

namespace
{
    direction_t turn_from_value(int8_t turn_value)
    {
        if (turn_value < 0 || turn_value > 2)
        {
            throw invalid_argument("Invalid turn value");
        }
        return static_cast<direction_t>(turn_value);
    }
} // namespace

ClientMessage::ClientMessage(session_t session_id, direction_t turn_direction, event_no_t next_expected_event_no,
                             const string &player_name) : ClientMessage(session_id, turn_direction, next_expected_event_no,
                                                                        string(player_name)) {}

ClientMessage::ClientMessage(session_t session_id, direction_t turn_direction, event_no_t next_expected_event_no,
                             string &&player_name) : session_id(session_id), turn_direction(turn_direction),
                                                     next_expected_event_no(next_expected_event_no), player_name(player_name)
{
}

ClientMessage::ClientMessage(const network::buffer_t &bytes)
{
    if (bytes.length() < client_mess_header_length)
    {
        throw invalid_argument("Unexpected end of message");
    }

    const char *data = bytes.data();
    size_t off = 0u;

    session_id = be64toh(*reinterpret_cast<const session_t *>(data + off));
    off += sizeof(session_t);

    turn_direction = turn_from_value(
        *reinterpret_cast<const int8_t *>(data + off));
    off += sizeof(direction_t);

    next_expected_event_no = be32toh(*reinterpret_cast<const event_no_t *>(data + off));
    off += sizeof(event_no_t);

    if (bytes.length() > client_mess_header_length)
    {
        player_name = string(data + off, bytes.size() - off);
    }
    else
    {
        player_name = "";
    }
}

network::buffer_t ClientMessage::to_bytes() const
{
    const size_t data_length =
        sizeof(session_t) + sizeof(direction_t) + sizeof(event_no_t);
    network::buffer_t bytes(data_length, '\0');
    char *data = const_cast<char *>(bytes.data());
    size_t off = 0u;

    *reinterpret_cast<session_t *>(data + off) = htobe64(session_id);
    off += sizeof(session_t);

    *reinterpret_cast<direction_t *>(data + off) = turn_direction;
    off += sizeof(direction_t);

    *reinterpret_cast<event_no_t *>(data + off) = htobe32(next_expected_event_no);
    copy(player_name.begin(), player_name.end(),
         back_inserter(bytes));
    return bytes;
}

session_t ClientMessage::get_session() const
{
    return session_id;
}

direction_t ClientMessage::get_turn_direction() const
{
    return turn_direction;
}

event_no_t ClientMessage::get_next_expected_event_no() const
{
    return next_expected_event_no;
}

const string &ClientMessage::get_player_name() const
{
    return player_name;
}

ServerMessage::ServerMessage(game_t game_id)
    : game_id(game_id)
{
    message_len = 0u;
}

void ServerMessage::add_event(shared_ptr<Event> event)
{
    if (message_len + event->get_len() > max_mess_data_length)
    {
        throw overflow_error("Adding event would cause overflow");
    }
    events.push_back(event);
    message_len += event->get_len();
}

network::buffer_t ServerMessage::to_bytes() const
{
    network::buffer_t bytes(sizeof(game_t), '\0');
    char *data = const_cast<char *>(bytes.data());
    *reinterpret_cast<game_t *>(data) = htobe32(game_id);
    for (const auto &event : events)
    {
        network::buffer_t e(event->to_bytes());
        copy(e.begin(), e.end(), back_inserter(bytes));
    }
    return bytes;
}

ServerMessage::ServerMessage(const network::buffer_t &bytes)
{
    game_id = be32toh(*reinterpret_cast<const game_t *>(bytes.data()));
    size_t off = sizeof(game_t);

    EventCreation event_factory;
    while (off < bytes.size())
    {
        try
        {
            shared_ptr<Event> event = event_factory.create_event(bytes, off);
            add_event(event);
            off += event->get_len();
        }
        catch (const CrcError &e)
        {
            break;
        }
        catch (const UnknownEventError &e)
        {
            cerr << "Unknown event" << endl;
            off += e.get_len();
        }
    }
}

game_t ServerMessage::get_game_id() const
{
    return game_id;
}

vector<shared_ptr<Event>>::iterator ServerMessage::begin()
{
    return events.begin();
}

vector<shared_ptr<Event>>::iterator ServerMessage::end()
{
    return events.end();
}
