#include "game_over.h"
#include "../../utils.h"

using namespace records;

network::buffer_t GameOver::get_data() const
{
    return network::buffer_t();
}

GameOver::GameOver(event_no_t event_no) : Event(event_no, event_t::GAME_OVER)
{
}

GameOver::GameOver(event_no_t event_no, size_t length) : Event(event_no, event_t::GAME_OVER)
{
    if (length != 0)
        exit_with_error("Unexpected argument");
}

string GameOver::to_string(__attribute__((unused)) const vector<string> &players) const
{
    return "GAME_OVER";
}

void GameOver::validate_event(
    __attribute__((unused)) pixel_t width,
    __attribute__((unused)) pixel_t height,
    __attribute__((unused)) size_t players_counterer) const
{
    return;
}