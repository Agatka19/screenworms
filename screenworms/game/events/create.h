#ifndef CREATE_H
#define CREATE_H


#include <memory>
#include "event.h"

using namespace std;

namespace records {
    class CrcError : public exception {
    };
    class UnknownEventError : public exception {
    private:
        size_t length;

    public:
        UnknownEventError(size_t length)  : length(length) {}
        size_t get_len() const  {
            return length;
        }
    };

    class EventCreation {
    public:
        shared_ptr<Event>
        create_event(const network::buffer_t &buffer, size_t off) const;
    };
}


#endif //CREATE_H
