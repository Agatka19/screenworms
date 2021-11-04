#include <boost/crc.hpp>
#include "create.h"
#include "new_game.h"
#include "pixel.h"
#include "player_eliminated.h"
#include "game_over.h"

using namespace records;

namespace
{
    crc32_t get_crc(const network::buffer_t &bytes, size_t buffer_offset, size_t size)
    {
        boost::crc_32_type res;
        res.process_bytes(bytes.data() + buffer_offset, size);
        return static_cast<crc32_t>(res.checksum());
    }
} // namespace

shared_ptr<Event> EventCreation::create_event(const network::buffer_t &buffer, size_t buffer_offset) const
{
    const char *data = buffer.data() + buffer_offset;
    len_t event_len = be32toh(*reinterpret_cast<const len_t *>(data));
    size_t data_off = sizeof(len_t);
    size_t data_len = event_len + sizeof(len_t) + sizeof(crc32_t);
    crc32_t crc = be32toh(*reinterpret_cast<const crc32_t *>(data + data_off + event_len));

    if (crc != get_crc(buffer, buffer_offset, event_len + sizeof(len_t)) || (data_len > buffer.size() - buffer_offset))
        throw CrcError();

    event_no_t event_no = be32toh(*reinterpret_cast<const event_no_t *>(data + data_off));
    data_off += sizeof(event_no_t);

    event_t event_type = *reinterpret_cast<const event_t *>(data + data_off);
    data_off += sizeof(event_type);

    const char *event_data = data + data_off;
    data_len = event_len - sizeof(event_type) - sizeof(event_no_t);

    switch (event_type)
    {
    case event_t::NEW_GAME:
        return make_shared<NewGame>(
            event_no, event_data, data_len);
    case event_t::PIXEL:
        return make_shared<Pixel>(
            event_no, event_data, data_len);
    case event_t::PLAYER_ELIMINATED:
        return make_shared<PlayerEliminated>(
            event_no, event_data, data_len);
    case event_t::GAME_OVER:
        return make_shared<GameOver>(
            event_no, data_len);
    default:
        throw UnknownEventError(
            sizeof(crc32_t) + sizeof(len_t) + event_len);
    }
}
