#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <string>
#include <netinet/in.h>
#include "../types.h"

namespace network
{
    class WouldBlock : public exception //EWOULDBLOCK
    {
    };

    class Sender
    {
    private:
        int sock;

    public:
        Sender(int sock)  : sock(sock) {}
        void send_message(sockaddr_storage *address, const buffer_t &buffer) const;
    };

    class Recipient
    {
    private:
        int sock;

    public:
        Recipient(int sock) ;
        buffer_t receive_message(sockaddr_storage *address);
    };

} // namespace network

#endif //CONVERSATION_H