#ifndef SIK_POLL_H
#define SIK_POLL_H

#include <stdexcept>
#include <sys/poll.h>
#include "../error.h"

using namespace std;

namespace network
{
    class PollTimeout : public exception
    {
    };

    class PollError : public exception
    {
    };

    template <size_t clients_max>
    class Poll
    {
        static_assert(clients_max > 0, "Poll size must be greater than 0");

    private:
        /****************************
        *         poll set
        ****************************/
        pollfd clients[clients_max];
        size_t clients_len;

        void client_set_default(pollfd &client) 
        {
            client.events = 0;
            client.revents = 0;
            client.fd = -1;
        }

        size_t get_client_index(int fd) const
        {
            for (size_t i = 0; i < clients_max; ++i)
                if (fd == clients[i].fd)
                    return i;
            throw out_of_range("fd is not in the poll");
        }

    public:
        Poll()  : clients_len(0)
        {
            for (pollfd &c : clients)
                client_set_default(c);
        }

        void add_fd(int fd, int events)
        {
            events = POLLIN;
            if (fd >= 0)
            {
                try
                {
                    size_t index = get_client_index(-1);
                    clients[index].events = events;
                    clients[index].fd = fd;
                    clients_len++;
                }
                catch (const out_of_range &)
                {
                    throw range_error("out of range");
                }
                try
                {
                    get_client_index(fd);
                    return;
                }
                catch (const out_of_range &)
                {
                }
            }
            else
                throw invalid_argument("fd has to be greater than 0");
        }

        pollfd &operator[](int fd)
        {
            if (fd >= 0)
                return clients[get_client_index(fd)];
            else
                throw invalid_argument("fd is invalid");
        }

        void wait(int timeout)
        {
            for (pollfd &client : clients)
                client.revents = 0;
            int result = poll(clients, clients_len, timeout);
            if (result < 0)
                throw PollError();
            if (result == 0)
                throw PollTimeout();
        }

        void remove_fd(int fd)
        {
            if (fd >= 0)
            {
                size_t index = get_client_index(fd);
                clients[index] = clients[clients_len];
                client_set_default(clients[clients_len]);
                clients_len--;
            }
            else
                throw invalid_argument("fd has to be greater than 0");
        }

        class iterator
        {
        private:
            pollfd *fd;

        public:
            iterator(pollfd *fd) : fd(fd) {}

            iterator operator++() 
            {
                ++fd;
                return iterator(fd);
            }

            const pollfd &operator*() const
            {
                return *fd;
            }

            bool operator!=(const iterator &other) const 
            {
                return fd != other.fd;
            }
        };

        iterator begin()
        {
            return Poll::iterator(clients);
        }

        iterator end()
        {
            return Poll::iterator(clients + clients_len);
        }
    };

} // namespace network

#endif
