#include "event.h"
#include <boost/crc.hpp>

using namespace records;

namespace
{
    crc32_t get_crc(const network::buffer_t &bytes)
    {
        boost::crc_32_type result;
        result.process_bytes(bytes.data(), bytes.size());
        return static_cast<crc32_t>(result.checksum());
    }
} // namespace

Event::Event(event_no_t event_no, records::event_t event_type)
    : event_no(event_no), event_type(event_type) {}

network::buffer_t records::Event::to_bytes() const
{
    network::buffer_t event_data(get_data());
    network::buffer_t bytes(event_header_length - sizeof(crc32_t), '\0');

    move(event_data.begin(), event_data.end(), back_inserter(bytes));

    const len_t length =
        static_cast<len_t>(bytes.size() - sizeof(len_t));

    char *data = const_cast<char *>(bytes.data());
    size_t off = 0u;

    *reinterpret_cast<len_t *>(data + off) = htobe32(length);
    off += sizeof(len_t);

    *reinterpret_cast<event_no_t *>(data + off) = htobe32(event_no);
    off += sizeof(event_no_t);

    *reinterpret_cast<records::event_t *>(data + off) = event_type;
    off = bytes.size();

    crc32_t crc = get_crc(bytes);
    bytes.resize(bytes.size() + sizeof(crc32_t), '\0');
    data = const_cast<char *>(bytes.data());

    *reinterpret_cast<crc32_t *>(data + off) = htobe32(crc);
    return bytes;
}

size_t Event::get_len() const
{
    return event_header_length;
}

event_no_t Event::get_event_no() const
{
    return event_no;
}

event_t Event::get_event_type() const
{
    return event_type;
}
