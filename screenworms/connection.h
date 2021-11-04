#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <memory>
#include "game/events/event.h"

using namespace std;
namespace gameServer {
    class Disconnector {
    public:
        virtual void disconnect_player(const string &name) = 0;
    };
}

namespace records {
    class EvMess {
    public:
        virtual void create_event_message(shared_ptr<Event> event) = 0;
    };
}


#endif //CONNECTION_H