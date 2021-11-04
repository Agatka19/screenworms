#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <vector>
#include "../../types.h"
using namespace std;
using namespace network;

namespace records
{
    constexpr size_t event_header_length = sizeof(len_t) + sizeof(event_t) +  sizeof(event_no_t) + sizeof(crc32_t);
    constexpr size_t max_event_length = max_mess_data_length - event_header_length;

    class Event
    {
    protected:
        /****************************
        *        event data
        ****************************/
        const event_no_t event_no;
        const event_t event_type;
        virtual buffer_t get_data() const  = 0;

    public:
        Event(event_no_t event_no, records::event_t event_type);

        buffer_t to_bytes() const ;
        virtual size_t get_len() const ;
        event_no_t get_event_no() const ;
        records::event_t get_event_type() const ;
        virtual string to_string(const vector<string> &players) const = 0;
        virtual void validate_event(pixel_t width, pixel_t height,
                                    size_t players_counter) const = 0;
    };
} // namespace records

#endif // EVENT_H
