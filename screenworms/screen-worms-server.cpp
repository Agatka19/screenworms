#include <iostream>
#include <ctime>
#include <csignal>
#include <memory>
#include <boost/program_options.hpp>

#include "server/server.h"
#include "utils.h"

using namespace std;
using namespace std;

namespace
{
    network::port_t server_port;
    records::GameParams game_params;
    unique_ptr<gameServer::Server> server;

    static constexpr auto max_rounds_per_second = 1000;
    static constexpr auto max_board_dimension = 10000;
    static constexpr auto max_turning_speed = 359;
    static constexpr auto max_connected_clients = 30;
    static constexpr auto min_players_number = 2;
    static constexpr auto client_timeout = chrono::seconds(2);

    void validate()
    {
        if (game_params.width > max_board_dimension || game_params.height > max_board_dimension)
            exit_with_error("Board is too big");
        if (game_params.rounds_per_sec > max_rounds_per_second)
            exit_with_error("Rounds_per_second option is too big");
        if (game_params.turning_speed > max_turning_speed)
            exit_with_error("Turning speed is too big");
        if (game_params.rounds_per_sec > max_rounds_per_second)
            exit_with_error("Rounds_per_second option is too big");
    }

    void print_usage() 
    {
        exit_with_error("Usage: ./screen-worms-server [-p n] [-s n] [-t n] [-v n] [-w n] [-h n]");
    }

    void parse_arguments(int argc, char *argv[])
    {
        int opt;
        while ((opt = getopt(argc, argv, "w:h:p:v:t:s:")) != -1)
        {
            switch (opt)
            {
            case 'w':
                game_params.width = to_number<decltype(game_params.width)>(
                    "-w", optarg, 1, max_board_dimension);
                break;

            case 'h':
                game_params.height = to_number<decltype(game_params.height)>(
                    "-h", optarg, 1, max_board_dimension);
                break;

            case 'p':
                game_params.port = to_number<decltype(game_params.port)>(
                    "-p", optarg, network::min_port, network::max_port);
                break;

            case 'v':
                game_params.rounds_per_sec = to_number<decltype(game_params.rounds_per_sec)>(
                    "-v", optarg, 1, max_rounds_per_second);
                break;

            case 't':
                game_params.turning_speed = to_number<decltype(game_params.turning_speed)>(
                    "-t", optarg, 1, max_turning_speed);
                break;

            case 's':
                game_params.seed = to_number<uint64_t>("-s", optarg);
                break;

            case '?':
                if (isprint(optopt))
                    print_usage();
                else
                    print_usage();
                break;
            }
        }
        if (optind != argc)
        {
            fprintf(stderr, "\nInvalid number of arguments.");
            print_usage();
        }
        validate();
    }

    void register_signals()
    {
        if (signal(SIGINT, [](int sig) {
                server->stop_server();
                cerr << "Signal " << sig << " stop server." << endl;
            }) == SIG_ERR)
        {
            throw runtime_error("Unable to register SIGINT signal");
        }
    }

    void server_init()
    {
        cout << "-------------- Server configuration --------------" << endl
             << "         Server port: " << game_params.port << endl
             << "          Dimensions: " << game_params.width << " x " << game_params.height << endl
             << "   Rounds per second: " << game_params.rounds_per_sec << endl
             << "       Turning speed: " << game_params.turning_speed << endl
             << "         Random seed: " << game_params.seed << endl
             << "--------------------------------------------------" << endl
             << endl;
    }
} // namespace

int main(int argc, char *argv[])
{
    try
    {
        parse_arguments(argc, argv);
        server_port = game_params.port;
        server = make_unique<gameServer::Server>(server_port, game_params);
        register_signals();

        server_init();
        server->run();
    }
    catch (const exception &e)
    {
        cout << "Server stopped with " << e.what() << endl;
        return RETURN_ERROR;
    }
    return RETURN_OK;
}
