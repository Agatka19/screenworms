#include <iostream>
#include <memory>
#include <csignal>
#include "client/client.h"
#include "utils.h"
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "types.h"

namespace
{
    string player_name;
    string game_server;
    network::port_t server_port = 2021;
    string gui_hostname = "localhost";
    network::port_t gui_port = 20210;
    unique_ptr<gameClient::Client> client;

    struct SocketAddress final
    {
        union
        {
            sockaddr addr;
            sockaddr_in addr_v4;
            sockaddr_in6 addr_v6;
        }; // because they have different sizes
        socklen_t addr_length;
        IpVersion ip_v;
    };

    bool resolve_address(const string &hostname, const port_t ip)
    {
        unique_ptr<SocketAddress> addr_ptr;
        addr_ptr.reset();
        addrinfo *result = nullptr;
        int s = getaddrinfo(hostname.empty() ? nullptr : hostname.c_str(),
                            nullptr, nullptr, &result);    
        if (s != 0 || result == nullptr)
        {
            return false;
        }
        auto ip_ver = result->ai_family == AF_INET ? IpVersion::IPv4 : result->ai_family == AF_INET6 ? IpVersion::IPv6 : IpVersion::None;
        if (ip_ver == IpVersion::None)
        {
            return false;
        }
        else
        {
            addr_ptr = make_unique<SocketAddress>();
            addr_ptr->ip_v = ip_ver;
            addr_ptr->addr_length = result->ai_addrlen;

            if (ip_ver == IpVersion::IPv6)
            {
                addr_ptr->addr_v6.sin6_port = htons(ip);
                addr_ptr->addr_v6.sin6_family = AF_INET6;
                memcpy(&addr_ptr->addr_v6.sin6_addr, &reinterpret_cast<sockaddr_in6 *>(result->ai_addr)->sin6_addr, sizeof(in6_addr));
            }
            else
            { // IPv4
                addr_ptr->addr_v4.sin_port = htons(ip);
                addr_ptr->addr_v4.sin_family = AF_INET;
                addr_ptr->addr_v4.sin_addr = reinterpret_cast<sockaddr_in *>(result->ai_addr)->sin_addr;
            }
        }

        freeaddrinfo(result);
        return true;
    }

    void register_signals()
    {
        if (signal(SIGINT, [](int sig) {
                client->stop_client();
                cerr << "Signal " << sig << " stop server." << endl;
            }) == SIG_ERR)
        {
            throw runtime_error("Unable to register SIGINT signal");
        }
    }

    void validate()
    {
        if (player_name.length() > 20)
        {
            exit_with_error("Player's name cannot be longer than 20 characters");
        }
        for (const char &letter : player_name)
        {
            if (letter < static_cast<char>(33) || letter > static_cast<char>(126))
            {
                exit_with_error("Please use only ASCII characters between 33 and 126 for player's name");
            }
        }
        if (server_port > network::max_port)
            exit_with_error("Wrong port");

        if (!resolve_address(game_server, server_port)){
            cout << game_server << ":::" << server_port << endl;
            exit_with_error("Cannot resolve server address properly");
        }

        if (!resolve_address(gui_hostname, gui_port))
            throw runtime_error("Not connected to GUI");

        cout << "Game server resolved as: " << game_server << endl;
        cout << "Server port resolved as: " << server_port << endl;
        cout << "Gui hostname resolved as: " << gui_hostname << endl;
        cout << "Gui port resolved as: " << gui_port << endl;
        if (player_name.empty())
        {
            cout << "Joining as an observer." << endl;
        }
        else
        {
            cout << "Joining as a player \"" << player_name << "\"." << endl;
        }
    }

    void print_usage() 
    {
        exit_with_error("Usage: ./screen-worms-client game_server [-n player_name] [-p n] [-i gui_server] [-r n]");
    }

    void parse_arguments(int argc, char **argv) 
    {
        if (argc < 1)
        {
            print_usage();
        }
        game_server = argv[1];

        int opt;
        while ((opt = getopt(argc, argv, "n:p:i:r:")) != -1)
        {
            try
            {
                switch (opt)
                {
                case 'n':
                    player_name = optarg;
                    break;

                case 'p':
                    server_port = atoi(optarg);
                    break;

                case 'i':
                    gui_hostname = optarg;
                    break;

                case 'r':
                    //atoi(optarg)
                    gui_port = atoi(optarg);
                    break;

                case '?':
                    if (isprint(optopt))
                        print_usage();
                    else
                        print_usage();
                    break;

                default:
                    print_usage();
                    break;
                }
            }
            catch (std::exception &exc)
            {
                exit_with_error(exc.what());
            }
        }
        if (optind != argc - 1) //exactly one non-option argument is expected
        {
            fprintf(stderr, "\nInvalid number of arguments.");
            print_usage();
        }
        validate();
    }
} // namespace

int main(int argc, char *argv[])
{
    parse_arguments(argc, argv);

    try
    {
        client = make_unique<gameClient::Client>(
            player_name, game_server, server_port, gui_hostname, gui_port);
        register_signals();
        client->run();
    }
    catch (const exception &e)
    {
        cerr << "Client stopped with message: " << e.what() << endl;
        return RETURN_ERROR;
    }
    return RETURN_OK;
}