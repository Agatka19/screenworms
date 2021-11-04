#ifndef SOCKET_H
#define SOCKET_H


#include <exception>
#include "../types.h"
#include <sys/socket.h>

namespace network {

    class Socket {
    private:
        int sock_dscrptr;
    public:
        Socket(int dom, int flag_t, int proto);
        ~Socket();
        int get_skt_dscrptr() const ;
        void bind_to_address(sockaddr *address, socklen_t address_length) const; //
        void bind_to_localhost(port_t port) const;
    };
}

#endif //SOCKET_H