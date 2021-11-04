#ifndef SNETWORKING_H
#define NETWORKING_H

#include <netinet/in.h>
#include <chrono>
#include "../connection.h"
#include "../types.h"
#include "../network/conversation.h"
#include <vector>
#include <queue>

using namespace std;
using namespace records;
using namespace chrono;

namespace gameServer
{
    using connection_t = time_point<system_clock>;
    constexpr seconds timeout_cst = seconds(2);
    class Networking
    {
    private:
        struct Client
        {
            /****************************
             *       client params
             ****************************/
            sockaddr_storage address;
            session_t session;
            string name;
            connection_t time_record;

            Client(sockaddr_storage address, session_t session,
                   string name, connection_t time_record);

            bool active_client(connection_t given_time) const ;
        };

        vector<Client> clients;
        Disconnector *listener;

    public:
        Networking(Disconnector *listener) ;

        string connect_client(sockaddr_storage address, session_t session,
                              connection_t now);

        void add_client(sockaddr_storage address, session_t session,
                        const string &name, connection_t now) ;
        queue<sockaddr_storage>
        get_connected_clients(connection_t now) ;
        void remove_inactive_clients(connection_t now) ;
    };

    class Buffer
    {
    public:
        struct Message
        {
            /****************************
             *      message objects
             ****************************/
            network::buffer_t buffer;
            queue<sockaddr_storage> addresses;

            Message(network::buffer_t, sockaddr_storage) ;
            Message(network::buffer_t, queue<sockaddr_storage> &&) ;
        };

    private:
        queue<Message> messages;

    public:
        bool is_empty() const ;
        void add(Message) ;
        Message pop();
    };

    class Events
    {
    private:
        vector<shared_ptr<records::Event>> events;

    public:
        void add_event(shared_ptr<records::Event> event);
        queue<shared_ptr<records::Event>>
        get_events(records::event_no_t next_expected_event_no) const ;
        void clean_events() ;
    };
} // namespace gameServer

#endif
