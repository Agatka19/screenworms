#ifndef GUI_H
#define GUI_H

#include "../types.h"

using namespace std;
using namespace records;

namespace gameClient
{
    class GuiCommunicaion
    {
    private:
        /****************************
        *       communication
        ****************************/
        int sock; // TCP socket for gui
        network::buffer_t buffer;
        size_t buffer_offset;
        direction_t direction;

    public:
        GuiCommunicaion(int sock, const string &host, network::port_t port);
        void send_event_gui(const string &event);
        void receive_event_gui();
        direction_t get_direction() const ;

    private:
        void decline(direction_t declined) ;
        void gui_connect(const string &host, network::port_t port);
    };
} // namespace gameClient

#endif //GUI_H