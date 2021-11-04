#include <iostream>
#include "conversation.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

using namespace network;

void Sender::send_message(sockaddr_storage *address, const network::buffer_t &buffer) const
{
    socklen_t len = 0;
    ssize_t length;
    sockaddr *addr = reinterpret_cast<sockaddr *>(address);
    auto fam = address->ss_family;

    if (fam == AF_INET)
        len = sizeof(sockaddr_in);
    else if (fam == AF_INET6)
        len = sizeof(sockaddr_in6);

    length = sendto(sock, buffer.data(), buffer.size(), 0, addr, len);

    if (length < 0 && errno == EWOULDBLOCK)
        throw WouldBlock();
    else if (length < 0)
        cerr << "Error while sending " << errno << endl;
}

Recipient::Recipient(int sock)  : sock(sock) {}

buffer_t Recipient::receive_message(sockaddr_storage *address) {
    memset(address, 0, sizeof(sockaddr_storage));
    sockaddr *sender = reinterpret_cast<sockaddr *>(address);
    socklen_t socklen = sizeof(sockaddr_storage);
    buffer_t buffer(max_message_length, '\0');
    char *data = const_cast<char *>(buffer.data());

    ssize_t length;
    length = recvfrom(sock, data, buffer.size(), 0, sender, &socklen);
    if (length < 0) {
        throw runtime_error("Error receiving data");
    }

    buffer.resize(static_cast<size_t>(length));
    return buffer;
}