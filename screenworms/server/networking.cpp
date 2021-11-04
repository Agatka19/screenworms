#include <stdexcept>
#include <tuple>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "networking.h"

using namespace gameServer;

Buffer::Message::Message(network::buffer_t buffer,
                         sockaddr_storage address) 
        : buffer(buffer) {
    addresses.push(address);
}

Buffer::Message::Message(network::buffer_t buffer,
                         queue<sockaddr_storage> &&addresses) 
        : buffer(buffer), addresses(addresses) {

}


bool Buffer::is_empty() const  {
    return messages.empty();
}

void Buffer::add(Buffer::Message message)  {
    messages.push(message);
}

Buffer::Message Buffer::pop() {
    if (messages.empty()) {
        throw out_of_range("Buffer is empty!");
    }
    Buffer::Message message = messages.front();
    messages.pop();
    return message;
}


namespace {
    bool operator ==(const sockaddr_storage &a, const sockaddr_storage &b) {
        return memcmp(&a, &b, sizeof(sockaddr_storage)) == 0;
    }
}

Networking::Client::Client(sockaddr_storage address,
                            records::session_t session, string name,
                            gameServer::connection_t time_record)
        : address(address), session(session), name(name), time_record(time_record) {
}

bool gameServer::Networking::Client::active_client(
        gameServer::connection_t given_time) const  {
    return given_time - time_record < timeout_cst;
}

Networking::Networking(Disconnector *listener) : listener(
        listener) {

}

string
Networking::connect_client(sockaddr_storage address, records::session_t session,
                        connection_t now) {
    remove_inactive_clients(now);

    for (auto client = clients.begin(); client != clients.end(); client++) {
        if (client->address == address) {
            if (client->session > session) {
                throw invalid_argument("Newer session exists");
            } else if (client->session < session) {
                listener->disconnect_player(client->name);
                clients.erase(client);
                throw out_of_range("Requested newer session");
            } else {
                client->time_record = now;
                return client->name;
            }
        }
    }

    throw out_of_range("Client does not exist");
}

void
Networking::add_client(sockaddr_storage address, records::session_t session,
                        const string &name, connection_t now)  {
    remove_inactive_clients(now);
    clients.push_back(Client(address, session, name, now));
}

queue<sockaddr_storage>
Networking::get_connected_clients(connection_t now)  {
    remove_inactive_clients(now);
    queue<sockaddr_storage> connected_clients;
    for (auto &client: clients) {
        connected_clients.push(client.address);
    }
    return connected_clients;
}

void Networking::remove_inactive_clients(connection_t now)  {
    for (const auto &client: clients) {
        if (!client.active_client(now)) {
            listener->disconnect_player(client.name);
        }
    }

    clients.erase(remove_if(clients.begin(), clients.end(),
                                 [&now](const auto &client) {
                                     return !client.active_client(now);
                                 }), clients.end());
}

void Events::add_event(shared_ptr<records::Event> event) {
    events.push_back(event);
}

queue<shared_ptr<records::Event>>
Events::get_events(records::event_no_t next_expected_event_no) const  {
    queue<shared_ptr<records::Event>> message_events;
    for (size_t i = next_expected_event_no; i < events.size(); i++) {
        message_events.push(events[i]);
    }
    return message_events;
}

void Events::clean_events()  {
    events.clear();
}