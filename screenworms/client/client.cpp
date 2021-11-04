#include <netdb.h>
#include <cstring>
#include <zconf.h>
#include <iostream>
#include <fcntl.h>
#include <chrono>
#include <boost/lexical_cast.hpp>
#include "client.h"
#include "../utils.h"

using namespace gameClient;
using namespace std;
using namespace network;
using namespace records;
using chrono::duration_cast;

Client::Client(const string &name, const string &hostname, port_t port, const string &gui_hostname, port_t gui_port) : name(name)
{
    /****************************
    *     init game params
    ****************************/
    event_no = 0;
    game_id = 0;
    width = 0;
    height = 0;

    calculate_session_id();
    open_sockets();
    adress_setup(hostname, port);

    /****************************
    *     connection params
    ****************************/
    gui = make_unique<GuiCommunicaion>(sock_gui, gui_hostname, gui_port);
    sender = make_unique<Sender>(sock_server);
    recipient = make_unique<Recipient>(sock_server);
    poll = make_unique<Poll<2>>();
    poll->add_fd(sock_server, POLLIN);
    poll->add_fd(sock_gui, POLLIN);
}

Client::~Client()
{
    if (close(sock_server) != 0 || close(sock_gui) != 0)
    {
        cerr << "Could not close socket properly" << endl;
    }
    client_thread.join();
}

void Client::open_sockets()
{
    sock_gui = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_gui < 0)
        exit_with_error("Cannot open socket for gui");
}

void Client::adress_setup(const string &hostname, port_t port)
{
    string port_buffer = boost::lexical_cast<string>(port);
    addrinfo info;
    addrinfo *res;

    memset(&info, 0, sizeof(info));
    info.ai_socktype = SOCK_DGRAM;
    info.ai_family = AF_UNSPEC;
    info.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo(hostname.c_str(), port_buffer.c_str(), &info, &res) != 0)
        exit_with_error("Cannot get server address");

    sock_server = socket(res->ai_family, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP);
    if (sock_server >= 0)
    {
        memcpy(&server_address, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }
    else
        exit_with_error("Cannot open socket for game server");
}

void Client::run()
{
    create_server_message();
    client_thread = thread([&]() {
        ms round_time(20);
        ms sleep_time(0);
        while (!should_stop)
        {
            auto beg = clock_time::now();
            create_server_message();
            auto end = clock_time::now();
            sleep_time += round_time;
            sleep_time -= duration_cast<ms>(end - beg);
            if (sleep_time > ms(0))
            {
                this_thread::sleep_for(sleep_time);
                sleep_time = ms(0);
            }
        }
    });

    while (!should_stop)
    {
        try
        {
            poll->wait(40);
        }
        catch (const PollError &)
        {
            continue;
        }
        catch (const PollTimeout &)
        {
            continue;
        }

        if (should_stop)
            return;

        if ((*poll)[sock_server].revents & POLLOUT)
            send_server_message();

        if ((*poll)[sock_server].revents & POLLIN)
            receive_message();

        if ((*poll)[sock_gui].revents & POLLIN)
        {
            try
            {
                gui->receive_event_gui();
            }
            catch (const runtime_error &e)
            {
                should_stop = true;
                throw runtime_error(e);
            }
        }

        if ((*poll)[sock_gui].revents & POLLOUT)
            send_event_to_gui();
    }
}

void Client::stop_client() 
{
    should_stop = true;
}

void Client::calculate_session_id() 
{
    ms now = duration_cast<ms>(clock_time::now().time_since_epoch());
    session = static_cast<session_t>(now.count());
}

void Client::create_server_message() 
{
    lock_guard<mutex> lock(mutex_messages);
    messages.push(make_unique<ClientMessage>(session, gui->get_direction(), event_no, name));
    (*poll)[sock_server].events = POLLIN | POLLOUT;
}

void Client::send_server_message()
{
    lock_guard<mutex> lock(mutex_messages);
    if (messages.size() == 0)
    {
        (*poll)[sock_server].events = POLLIN;
        return;
    }
    try
    {
        sender->send_message(&server_address, messages.front()->to_bytes());
        messages.pop();
    }
    catch (const WouldBlock &)
    {
    }
}

void Client::send_event_to_gui()
{
    if (received_events.size() == 0)
    {
        (*poll)[sock_gui].events = POLLIN;
        return;
    }
    try
    {
        gui->send_event_gui(received_events.front());
        received_events.pop();
    }
    catch (const runtime_error &e)
    {
        should_stop = true;
        throw runtime_error(e);
    }
    catch (const WouldBlock &)
    {
    }
}

void Client::receive_message()
{
    buffer_t buff = recipient->receive_message(&server_address);
    unique_ptr<ServerMessage> mess;
    try
    {
        mess = make_unique<ServerMessage>(buff);
    }
    catch (const invalid_argument &e)
    {
        stop_client();
        exit_with_error("Invalid argument");
    }

    if (game_id != mess->get_game_id())
        event_no = 0;
    game_id = mess->get_game_id();

    for (const auto &event : *mess)
    {
        if (event_no == event->get_event_no())
        {
            auto type = event->get_event_type();
            if (type == event_t::GAME_OVER)
            {
                continue;
            }
            else if (type == event_t::NEW_GAME)
            {
                players_initialize(reinterpret_cast<NewGame *>(event.get()));
            }
            try
            {
                event->validate_event(width, height, players.size());
            }
            catch (const invalid_argument &e)
            {
                stop_client();
                exit_with_error("Invalid argument");
            }
            received_events.push(event->to_string(players));
            (*poll)[sock_gui].events = POLLIN | POLLOUT;
            ++event_no;
        }
        else if (event_no < event->get_event_no())
            break;
        else
            continue; // ?
    }
}

void Client::players_initialize(NewGame *event)
{
    height = event->get_height();
    width = event->get_width();
    players.clear();
    for (const string &player : *event)
        players.push_back(player);
}
