#ifndef SIK_records_PLAYER_H
#define SIK_records_PLAYER_H


#include "types.h"

namespace records {
    using coordinate_t = double;
    using dir_angle_t = int;

    class Worm {
    private:
        /****************************
        *         worm params
        ****************************/
        coordinate_t x;
        coordinate_t y;
        dir_angle_t dir_angle;
        direction_t direction;
        bool in_game;

    public:
        Worm(coordinate_t x, coordinate_t y, dir_angle_t dir_angle) ;

        /****************************
        *     coordinates getter
        ****************************/
        coordinates_t get_position() const ;


        /****************************
        *      worm actions
        ****************************/
        void turn(direction_t direction) ;
        bool move_by_unit(turn_speed_t speed) ;
        bool still_playing() const ;
        void eliminate() ;
    };
}


#endif
