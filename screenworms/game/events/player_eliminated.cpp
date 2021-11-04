#include <sstream>
#include "player_eliminated.h"
#include "../../utils.h"

#define sp sizeof(player_number_t)

using namespace records;

PlayerEliminated::PlayerEliminated(event_no_t event_no, player_number_t player_number) : Event(event_no, event_t::PLAYER_ELIMINATED), player_number(player_number)
{
}

network::buffer_t PlayerEliminated::get_data() const
{
    network::buffer_t bytes(sp, '\0');
    char *data = const_cast<char *>(bytes.data());
    *reinterpret_cast<player_number_t *>(data) = player_number;
    return bytes;
}

size_t PlayerEliminated::get_len() const
{
    return Event::get_len() + sp;
}

PlayerEliminated::PlayerEliminated(event_no_t event_no, const char *data, size_t length) : Event(event_no, event_t::PLAYER_ELIMINATED)
{
    if (length != sp)
        exit_with_error("Error: Length of data");
    player_number = *reinterpret_cast<const player_number_t *>(data);
}

string PlayerEliminated::to_string(const vector<string> &players) const
{
    ostringstream os;
    if (players.size() < player_number)
        exit_with_error("No such player");
    os << "PLAYER_ELIMINATED " << players[player_number] << "\n";
    return os.str();
}

void PlayerEliminated::validate_event(__attribute__((unused)) pixel_t width, __attribute__((unused)) pixel_t height,
                                      size_t players_counter) const
{
    if (player_number >= players_counter)
        exit_with_error("No such player");
}
