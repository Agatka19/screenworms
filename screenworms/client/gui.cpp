#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "gui.h"
#include "../utils.h"

using namespace gameClient;
using namespace records;

GuiCommunicaion::GuiCommunicaion(int sock, const string &hostname, network::port_t port) : sock(sock), direction(direction_t::STRAIGHT)
{
    buffer = string(20, 0);
    buffer_offset = 0;
    gui_connect(hostname, port);
}

void GuiCommunicaion::gui_connect(const string &hostname, network::port_t port)
{
    string port_str = boost::lexical_cast<string>(port);
    addrinfo info;
    addrinfo *res;

    memset(&info, 0, sizeof(info)); //addrinfo
    info.ai_socktype = SOCK_STREAM;
    info.ai_family = AF_INET;
    info.ai_protocol = IPPROTO_TCP;
    info.ai_flags = 0;

    auto got = getaddrinfo(hostname.c_str(), port_str.c_str(), &info, &res);
    if (got != 0)
        exit_with_error("Cannot reach gui");

    auto connected = connect(sock, res->ai_addr, res->ai_addrlen);
    if (connected != 0)
        throw runtime_error("Not connected to gui");

    freeaddrinfo(res);
}

void GuiCommunicaion::send_event_gui(const string &event)
{
    auto sent = send(sock, event.data(), event.length(), 0);
    if (sent < 0)
        throw runtime_error("Not connected to gui");
}

void GuiCommunicaion::decline(direction_t declined) 
{
    if (direction == declined)
        direction = direction_t::STRAIGHT;
}

void GuiCommunicaion::receive_event_gui()
{
    char *data = const_cast<char *>(buffer.data());
    ssize_t length = read(sock, data + buffer_offset, buffer.size() - 1 - buffer_offset);
    if (length <= 0)
        throw runtime_error("Not connected to gui");

    buffer_offset += length;
    string action = "";

    for (size_t i = 0; i < buffer_offset; i++)
    {
        if (buffer[i] == '\n')
        {
            action = buffer.substr(0, i);
            buffer = buffer.substr(i + 1);
            buffer.resize(20, 0);
            buffer_offset -= i + 1;
            break;
        }
    }

    if (action == "")
        return;

    else if (action == "RIGHT_KEY_DOWN")
        direction = direction_t::RIGHT;

    else if (action == "RIGHT_KEY_UP")
        decline(direction_t::RIGHT);

    else if (action == "LEFT_KEY_DOWN")
        direction = direction_t::LEFT;

    else if (action == "LEFT_KEY_UP")
        decline(direction_t::LEFT);
}

direction_t GuiCommunicaion::get_direction() const 
{
    return direction;
}


// GuiCommunicaion::GuiCommunicaion(int sock, const string &hostname, network::port_t port)
//         : sock(sock), direction(records::direction_t::STRAIGHT) {
//     gui_connect(hostname, port);
//     buffer = string(20, 0);
//     buffer_offset = 0;
// }

// void GuiCommunicaion::gui_connect(const string &hostname, network::port_t port) {
//     addrinfo hints, *result;
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_flags = 0;
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_protocol = IPPROTO_TCP;

//     string port_str = boost::lexical_cast<string>(port);

//     if (getaddrinfo(hostname.c_str(), port_str.c_str(), &hints, &result) != 0) {
//         throw invalid_argument("Error getting GUI address");
//     }

//     if (connect(sock, result->ai_addr, result->ai_addrlen) != 0) {
//         throw runtime_error("GUI Connection error");
//     }
//     freeaddrinfo(result);
// }

// void GuiCommunicaion::send_event_gui(const string &event) {
//     if (send(sock, event.data(), event.length(), 0) < 0) {
//         throw runtime_error("GUI Connection error");
//     }
// }

// void GuiCommunicaion::receive_event_gui() {
//     char *data = const_cast<char *>(buffer.data());
//     ssize_t length;
//     if ((length = read(sock, data + buffer_offset, buffer.size() - buffer_offset - 1)) <= 0) {
//         throw runtime_error("GUI Connection error");
//     }
//     buffer_offset += length;
//     string action("");

//     for (size_t i = 0; i < buffer_offset; i++) {
//         if (buffer[i] == '\n') {
//             action = buffer.substr(0, i);
//             buffer = buffer.substr(i + 1);
//             buffer.resize(20, 0);
//             buffer_offset -= i + 1;
//             break;
//         }
//     }

//     if (action == "") {
//         return;
//     }

//     if (action == "LEFT_KEY_DOWN") {
//         cout << "LEFT_KEY_DOWN" << endl;
//         direction = records::direction_t::LEFT;
//     } else if (action == "LEFT_KEY_UP") {
//         cout << "LEFT_KEY_UP" << endl;
//         decline(records::direction_t::LEFT);
//     } else if (action == "RIGHT_KEY_DOWN") {
//         cout << "RIGHT_KEY_DOWN" << endl;
//         direction = records::direction_t::RIGHT;
//     } else if (action == "RIGHT_KEY_UP") {
//         cout << "RIGHT_KEY_ up" << endl;
//         decline(records::direction_t::RIGHT);
//     }
// }

// records::direction_t GuiCommunicaion::get_direction() const  {
//     return direction;
// }

// void GuiCommunicaion::decline(records::direction_t declined)  {
//     if (direction == declined) {
//         direction = records::direction_t::STRAIGHT;
//     }
// }

