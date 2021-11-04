#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <zconf.h>
#include <iostream>
#include "socket.h"
#include "../utils.h"

using namespace network;

Socket::Socket(int dom, int flag_t, int proto)
{
    sock_dscrptr = socket(dom, flag_t, proto);
    if (sock_dscrptr < 0)
        exit_with_error("Cannot create socket");
}

Socket::~Socket()
{
    if (close(sock_dscrptr) < 0)
        exit_with_error("Cannot close socket");
}

int Socket::get_skt_dscrptr() const 
{
    return sock_dscrptr;
}

void Socket::bind_to_localhost(port_t port) const
{
    sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_port = htons(port);
    addr.sin6_family = AF_INET6;
    if (bind(sock_dscrptr, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        exit_with_error("Cannot bind to socket");
}
