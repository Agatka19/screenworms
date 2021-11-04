#include "server.h"
#include "../game/message.h"
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <fcntl.h>
#include "../utils.h"

using namespace gameServer;
using namespace std;

Server::Server(network::port_t port, const records::GameParams &game_params) : Server::Server(port, records::GameParams(game_params)) {}


Server::Server(network::port_t port, records::GameParams &&game_params) {
    events = make_unique<Events>();
    game = make_unique<records::Game>(game_params, this);
    connections = make_unique<Networking>(this);
    messages = make_unique<Buffer>();
    
    socket = make_unique<network::Socket>(AF_INET6, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP);
    socket->bind_to_localhost(port);

    sender = make_unique<network::Sender>(socket->get_skt_dscrptr());
    recipient = make_unique<network::Recipient>(socket->get_skt_dscrptr());

    poll = make_unique<network::Poll<1>>();
    poll->add_fd(socket->get_skt_dscrptr(), POLLIN | POLLOUT);
}

void Server::run() {
    should_stop = false;
    while (!should_stop) {
        try {
            poll->wait(2000);
        } catch (const network::PollTimeout &) {
            connection_t curr_time = chrono::high_resolution_clock::now();
            connections->remove_inactive_clients(curr_time);
        } catch (const network::PollError &) {
            continue;
        }

        if (should_stop)
            return;

        if ((*poll)[socket->get_skt_dscrptr()].revents & POLLIN) {
            receive_message();
        }
        if ((*poll)[socket->get_skt_dscrptr()].revents & POLLOUT) {
            send_message();
        }
    }
}

void Server::stop_server()  {
    should_stop = true;
}

void Server::create_event_message(shared_ptr<records::Event> event) {
    {
        lock_guard<mutex> guard(mutex_events);
        if (event->get_event_type() == records::event_t::GAME_OVER) {
            events->clean_events();
        } else {
            events->add_event(event);
        }
    }

    connection_t curr_time = chrono::high_resolution_clock::now();
    queue<sockaddr_storage> clients;
    records::ServerMessage message(game->get_id());

    message.add_event(event);
    {
        lock_guard<mutex> guard(connections_mutex);
        clients = connections->get_connected_clients(curr_time);
    }

    add_message(Buffer::Message(message.to_bytes(), move(clients)));
}

void Server::disconnect_player(const string &name) {
    if (name == "") {
        cout << "Observer disconnected" << endl;
        return;
    }
    game->remove_player(name);
}

bool Server::sending_possible()  {
    lock_guard<mutex> guard(mutex_messages);

    while (!messages->is_empty() && current_addresses.empty()) {
        Buffer::Message message = messages->pop();
        current_message = message.buffer;
        current_addresses = message.addresses;
    }

    if (current_addresses.size() == 0) {
        (*poll)[socket->get_skt_dscrptr()].events = POLLIN;
        return false;
    }
    return true;
}

void Server::send_message() {
    if (!sending_possible()) {
        return;
    }

    try {
        sockaddr_storage address = current_addresses.front();
        sender->send_message(&address, current_message);
        current_addresses.pop();
    } catch (const network::WouldBlock &) {
        return;
    }
}

void Server::receive_message() {
    connection_t curr_time = chrono::high_resolution_clock::now();
    sockaddr_storage address = sockaddr_storage();
    network::buffer_t buffer = recipient->receive_message(&address);
    shared_ptr<records::ClientMessage> message;

    try {
        message = make_shared<records::ClientMessage>(buffer);
    } catch (const invalid_argument &e) {
        cerr << "Invalid message received: " << e.what() << endl;
        return;
    }

    try {
        game_action(message, address, curr_time);
    } catch (const invalid_argument &e) {
        cerr << e.what() << endl;
    } catch (const out_of_range &) {
        player_connect(message, address, curr_time);
    }
}

void Server::create_message(sockaddr_storage address,
                          records::event_no_t event_no) {
    records::ServerMessage message(game->get_id());
    queue<shared_ptr<records::Event>> message_events;
    {
        lock_guard<mutex> guard(mutex_events);
        message_events = events->get_events(event_no);
    }
    if (message_events.size() == 0) {
        return;
    }

    while (!message_events.empty()) {
        try {
            message.add_event(message_events.front());
            message_events.pop();
        } catch (const overflow_error &) {
            add_message(Buffer::Message(message.to_bytes(), address));
            message = records::ServerMessage(game->get_id());
        }
    }
    add_message(Buffer::Message(message.to_bytes(), address));
}

void Server::add_message(Buffer::Message message)  {
    lock_guard<mutex> guard(mutex_messages);
    messages->add(message);
    (*poll)[socket->get_skt_dscrptr()].events = POLLIN | POLLOUT;
}

void Server::game_action(shared_ptr<records::ClientMessage> message,
                       sockaddr_storage address, connection_t curr_time) {
    string player;

    {
        lock_guard<mutex> guard(connections_mutex);
        player = connections->connect_client(address, message->get_session(), curr_time);
    }

    if (player != message->get_player_name()) {
        exit_with_error("Message did not reach the desired recipient");
    }

    if (player != "") {
        game->player_move(player, message->get_turn_direction());
    }
    create_message(address, message->get_next_expected_event_no());
}

void Server::player_connect(shared_ptr<records::ClientMessage> message,
                        sockaddr_storage address, connection_t curr_time)  {
    try {
        if (message->get_player_name() != "") {
            game->add_player(message->get_player_name());
            cout << "Player `" << message->get_player_name()
                      << "` connected" << endl;
        } else {
            cout << "Observer connected" << endl;
        }

        records::session_t session = message->get_session();
        string name = message->get_player_name();

        {
            lock_guard<mutex> guard(connections_mutex);
            connections->add_client(address, session, name, curr_time);
        }

        game_action(message, address, curr_time);
    } catch (const invalid_argument &) {
        return;
    }
}