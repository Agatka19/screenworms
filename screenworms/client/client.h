#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <netinet/in.h>
#include <queue>
#include <list>
#include "gui.h"
#include <mutex>
#include <thread>
#include "../types.h"
#include "../network/poll.h"
#include "../network/conversation.h"
#include "../game/message.h"
#include "../game/events/new_game.h"

using namespace std;
using namespace records;

namespace gameClient
{
    class Client
    {
    private:
        /****************************
        *           game
        ****************************/
        vector<string> players;
        pixel_t width;
        pixel_t height;

        /****************************
        *           socket
        ****************************/
        sockaddr_storage server_address;
        int sock_server;
        int sock_gui;
        bool should_stop = false;

        /****************************
        *       communication
        ****************************/
        unique_ptr<network::Poll<2>> poll;
        unique_ptr<network::Sender> sender;
        unique_ptr<network::Recipient> recipient;
        unique_ptr<GuiCommunicaion> gui;
        queue<unique_ptr<ClientMessage>> messages;
        queue<string> received_events;
        mutex mutex_messages;
        thread client_thread;

        /****************************
        *          message
        ****************************/
        game_t game_id;
        session_t session;
        string name;
        event_no_t event_no;

    public:
        Client(const string &name,
               const string &host, network::port_t port,
               const string &gui_hostname, network::port_t gui_port);
        ~Client();

    private:
        void open_sockets();
        void adress_setup(const string &host, network::port_t port);

    public:
        void run();
        void stop_client() ;

    private:
        void calculate_session_id() ;
        void create_server_message() ;
        void send_server_message();
        void send_event_to_gui();
        void receive_message();
        void players_initialize(records::NewGame *event);
    };
} // namespace gameClient

#endif //CLIENT_H