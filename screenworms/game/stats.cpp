#include "stats.h"
#include "events/new_game.h"
#include "events/player_eliminated.h"
#include "events/pixel.h"
#include "events/game_over.h"
#include <algorithm>
#include <queue>

using namespace records;
using namespace std;

Game::Game(const GameParams &game_params,
           EvMess *listener) 
        : Game(GameParams(game_params), listener) {

}

Game::Game(GameParams &&game_params,
           EvMess *listener) 
        : game_params(game_params), listener(listener) {
    running = false;
    random = make_unique<RandomNumberGenerator>(game_params.seed);
    board = make_unique<Board>(game_params.width, game_params.height);
    initialize_new_game();
}

void Game::initialize_new_game() {
    game_id = random->next();
    ready_players_no = 0;
    event_no = 1;

    for (auto &player: players) {
        player.second.ready = false;
        player.second.in_game = false;
    }
}

void Game::add_player(const string &name) {
    lock_guard<mutex> guard(players_mutex);
    // if (name.length() > MAX_PLAYER_NAME_LENGTH) {
    //     throw length_error(
    //             "Player name cannot be longer than 64 characters");
    // }

    if (players.count(name)) {
        throw invalid_argument("Player already in game");
    }
    players.insert(make_pair(name, Player()));
}

void Game::player_move(const string &name, direction_t direction) {
    lock_guard<mutex> guard(players_mutex);
    if (players.count(name)) {
        if (running) {
            action_react_gameon(players[name], direction);
        } else {
            action_react_gameoff(players[name], direction);
        }
    }
}

void Game::action_react_gameoff(Player &player, direction_t direction)  {
    if (player.ready || direction == direction_t::STRAIGHT) {
        return;
    }
    player.ready = true;
    ++ready_players_no;
    start();
}

void
Game::action_react_gameon(const Player &player, direction_t direction)  {
    if (!player.in_game) {
        return;
    }
    worms[player.player_number]->turn(direction);
}

void Game::remove_player(const string &name)  {
    lock_guard<mutex> guard(players_mutex);
    auto player = players.find(name);
    if (player == players.end()) {
        return;
    }
    if (player->second.ready) {
        --ready_players_no;
    }
    players.erase(player);
    if (!running) {
        start();
    }
    cout << "Player `" << name << "` disconnected from the game" << endl;
}

void Game::start()  {
    if (players.size() < 2 || ready_players_no < players.size()) {
        return;
    }
    running = true;

    new_game();
    game_thread = thread([&]() {
        game_loop();
    });
    game_thread.detach();
}

game_t Game::get_id() const  {
    return game_id;
}

void Game::iterate()  {
    if (worms_in_game_no <= 1) {
        listener->create_event_message(make_unique<GameOver>(event_no++));
        running = false;
        initialize_new_game();
        return;
    }

    for (player_number_t i = 0; i < worms.size(); i++) {
        Worm *worm = worms[i].get();
        if (!worm->still_playing() || !worm->move_by_unit(game_params.turning_speed)) {
            continue;
        }
        listener->create_event_message(situate_worm(worm, i));
    }
}

void Game::new_game()  {
    player_number_t player_number = 0u;
    worms_in_game_no = 0u;

    board->clear();
    worms.clear();

    queue<unique_ptr<Event>> events;
    unique_ptr<NewGame> event_new_game =
            make_unique<NewGame>(
                    game_params.width, game_params.height, 0u);

    for (auto &player: players) {
        try {
            event_new_game->add_player(player.first);
            player.second.player_number = player_number;
            player.second.in_game = true;

            unique_ptr<Worm> worm = create_worm();
            worms_in_game_no++;

            events.push(situate_worm(worm.get(), player_number));
            worms.push_back(move(worm));

            player_number++;
        } catch (const overflow_error &) {
            // Player name doesn't fit in the new game message stop adding
            break;
        } catch (const invalid_argument &) {
            // Player name is not a valid name skip
            continue;
        }
    }

    listener->create_event_message(move(event_new_game));
    while (!events.empty()) {
        listener->create_event_message(move(events.front()));
        events.pop();
    }
}

unique_ptr<Worm> Game::create_worm()  {
    return make_unique<Worm>(
            random->next() % game_params.width + 0.5,
            random->next() % game_params.height + 0.5,
            random->next() % 360
    );
}

unique_ptr<Event>
Game::situate_worm(Worm *worm, player_number_t player_number)  {
    coordinates_t worm_position = worm->get_position();
    if (board->is_empty(worm_position)) {
        board->eat(worm_position);
        return make_unique<Pixel>(
                event_no++, player_number, worm_position);
    } else {
        worm->eliminate();
        worms_in_game_no--;
        return make_unique<PlayerEliminated>(event_no++, player_number);
    }
}

Game::~Game() {
    running = false;
}

void Game::game_loop() {
    using chrono::microseconds;
    using chrono::duration_cast;
    using chrono::high_resolution_clock;

    microseconds round_time(1000000 / game_params.rounds_per_sec);
    microseconds sleep_time(0);
    while (running) {
        auto start = high_resolution_clock::now();
        iterate();
        auto end = high_resolution_clock::now();
        sleep_time += round_time;
        sleep_time -= duration_cast<microseconds>(end - start);
        if (sleep_time > microseconds(0)) {
            this_thread::sleep_for(sleep_time);
            sleep_time = microseconds(0);
        }
    }

    cout << "Game over" << endl;
}