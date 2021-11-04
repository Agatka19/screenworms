#include <sstream>
#include "new_game.h"
#include "../../utils.h"

using namespace records;

NewGame::NewGame(pixel_t max_x, pixel_t max_y, event_no_t event_no) : Event(event_no, event_t::NEW_GAME), max_x(max_x), max_y(max_y)
{
    players_len = 0;
}

network::buffer_t NewGame::get_data() const
{
    size_t off = 0;
    network::buffer_t bytes(players_len + new_game_head_len, '\0');
    char *data = const_cast<char *>(bytes.data());

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(max_x);
    off += sizeof(pixel_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(max_y);
    off += sizeof(pixel_t);

    for (const auto &player : players)
    {
        copy(player.begin(), player.end(), bytes.begin() + off);
        off += player.length();
        bytes[off++] = '\0';
    }
    return bytes;
}

void NewGame::add_player(const string &player)
{
    add_player(string(player));
}

void NewGame::add_player(string &&player)
{
    if (players_len + player.length() + 1 > max_event_length)
        exit_with_error("Too many players");

    players_len += player.length() + 1;
    players.insert(player);
}

size_t NewGame::get_len() const
{
    return Event::get_len() + players_len + new_game_head_len;
}

NewGame::NewGame(event_no_t event_no, const char *data, size_t length) : Event(event_no, event_t::NEW_GAME)
{
    size_t off = 0;
    if (length < new_game_head_len + 4)
        exit_with_error("Error: Length of data");

    max_x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    max_y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += (2 * sizeof(pixel_t));

    string last_player = "";

    while (off < length)
    {
        string player_name(data + off);
        if (player_name < last_player)
            exit_with_error("This is not an alphabetical order");
        add_player(player_name);
        last_player = player_name;
        off += player_name.length() + 1;
    }
}

pixel_t NewGame::get_width() const
{
    return max_x;
}

pixel_t NewGame::get_height() const
{
    return max_y;
}

set<string>::const_iterator NewGame::begin() const
{
    return players.begin();
}

set<string>::const_iterator NewGame::end() const
{
    return players.end();
}

string
NewGame::to_string(
    const vector<string> &players) const
{
    ostringstream os;
    os << "NEW_GAME " << max_x << " " << max_y;
    for (const auto &player : players)
    {
        os << " " << player;
    }
    os << "\n";
    return os.str();
}

void NewGame::validate_event(
    __attribute__((unused)) pixel_t width,
    __attribute__((unused)) pixel_t height,
    __attribute__((unused)) size_t players_counter) const
{
    return;
}