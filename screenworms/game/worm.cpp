#include "worm.h"
#include <cmath>
#include <tuple>
#include "../utils.h"
#include "../types.h"

using namespace records;

#define deg2rad M_PI / 180

Worm::Worm(
    coordinate_t x, coordinate_t y, dir_angle_t dir_angle) 
     : x(x), y(y), dir_angle(dir_angle), 
    direction(direction_t::STRAIGHT), in_game(true) 
{
}

coordinates_t Worm::get_position() const 
{
    return make_pair(static_cast<pixel_t>(x), static_cast<pixel_t>(y));
}

void Worm::turn(direction_t direction) 
{
    this->direction = direction;
}

bool Worm::move_by_unit(turn_speed_t speed) 
{
    coordinates_t last_td = get_position();

    if (direction == LEFT)
    {
        dir_angle -= speed;
        dir_angle %= 360;
        if (dir_angle < 0)
        {
            dir_angle += 360;
        }
    }
    else if (direction == RIGHT)
    {
        dir_angle += speed;
        dir_angle %= 360;
        if (dir_angle >= 360)
        {
            dir_angle -= 360;
        }
    }

    x += cos(dir_angle * deg2rad);
    y += sin(dir_angle * deg2rad);

    return last_td != get_position();
}

void Worm::eliminate() 
{
    in_game = false;
}

bool Worm::still_playing() const 
{
    return in_game;
}
