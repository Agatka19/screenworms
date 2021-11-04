#ifndef SIK_records_SERVER_H
#define SIK_records_SERVER_H

#include <netinet/in.h>
#include <mutex>
#include "../network/socket.h"
#include "../network/poll.h"
#include "../game/message.h"
#include "../game/stats.h"
#include "networking.h"

using namespace std;
using namespace records;

namespace gameServer
{
    class Server : public records::EvMess, public Disconnector
    {
    private:
        bool should_stop;

        /****************************
        *         connection
        ****************************/
        unique_ptr<network::Poll<1>> poll;
        unique_ptr<network::Socket> socket;
        unique_ptr<Networking> connections;
        mutex connections_mutex;

        /****************************
        *           game
        ****************************/
        unique_ptr<records::Game> game;
        unique_ptr<Events> events;
        mutex mutex_events;

        /****************************
        *         messages
        ****************************/
        unique_ptr<Buffer> messages;
        mutex mutex_messages;
        unique_ptr<network::Sender> sender;
        queue<sockaddr_storage> current_addresses;
        unique_ptr<network::Recipient> recipient;
        network::buffer_t current_message;

    public:
        Server(network::port_t port, const records::GameParams &game_params);
        Server(network::port_t port = 2021,
               records::GameParams &&game_params = records::GameParams());

        void run();
        void stop_server() ;
        void create_event_message(shared_ptr<records::Event> event) override;
        void disconnect_player(const string &name) override;

    private:
        void receive_message();
        void create_message(sockaddr_storage address, records::event_no_t event_no);
        void add_message(Buffer::Message message) ; //to the buffer
        bool sending_possible() ;
        void send_message();
        void game_action(shared_ptr<records::ClientMessage> message,
                         sockaddr_storage address, connection_t curr_time);
        void player_connect(shared_ptr<records::ClientMessage> message,
                            sockaddr_storage address, connection_t curr_time) ;
    };

} // namespace gameServer

#endif
