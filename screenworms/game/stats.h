#ifndef SIK_records_GAME_H
#define SIK_records_GAME_H

#include <ctime>
#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include "types.h"
#include "random_generator.h"
#include "worm.h"
#include "board.h"
#include "../connection.h"

using namespace std;

namespace records {

    struct GameParams {
        network::port_t port = 2021;
        turn_speed_t turning_speed = 6;
        rounds_t rounds_per_sec = 50;
        pixel_t width =  640;
        pixel_t height = 480;
        rand_t seed = static_cast<rand_t>(time(NULL));
    };

    struct Player {
        player_number_t player_number = 0;
        bool in_game = false;
        bool ready = false;
    };

    class Game {
    private:
        /****************************
        *        game params
        ****************************/
        GameParams game_params;
        game_t game_id;
        bool running;
        event_no_t event_no;
        thread game_thread;
        unique_ptr<RandomNumberGenerator> random;
        unique_ptr<Board> board;


        /****************************
        *          players
        ****************************/
        map<string, Player> players;
        mutex players_mutex;
        uint8_t ready_players_no;
        EvMess *listener;


        /****************************
        *          worms
        ****************************/
        vector<unique_ptr<Worm>> worms;
        size_t worms_in_game_no;

    public:
        Game(const GameParams &game_params, EvMess *listener) ;
        Game(GameParams &&game_params, EvMess *listener) ;
        ~Game();

        /****************************
        *          player
        ****************************/
        void add_player(const string &name);
        void remove_player(const string &name) ;
        void player_move(const string &name, direction_t direction);
        game_t get_id() const ;

    private:
        void action_react_gameoff(Player &player, direction_t direction) ;
        void action_react_gameon(const Player &player, direction_t direction) ;

        /****************************
        *          new game
        ****************************/
        void initialize_new_game();
        void start() ;
        void game_loop();
        void iterate() ;
        void new_game() ;

        /****************************
        *           worms
        ****************************/
        unique_ptr<Worm> create_worm() ;
        unique_ptr<Event> situate_worm(Worm *worm, player_number_t player_number) ;
    };
}

#endif