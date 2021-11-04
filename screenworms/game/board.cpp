#include "board.h"

using namespace records;

#define EMPTY false
#define NOT_EMPTY true

Board::Board(pixel_t width, pixel_t height) : width(width), height(height) {
    fields = vector<vector<bool>>(width);
    for (pixel_t x = 0; x < width; x++)
        fields[x] = vector<bool>(height);
    clear();
}

void Board::clear()  {
    for (pixel_t x = 0; x < width; x++) {
        for (pixel_t y = 0; y < height; y++)
            fields[x][y] = EMPTY;
    }
}

void Board::eat(coordinates_t coordinates)  {
    fields[coordinates.first][coordinates.second] = NOT_EMPTY;
}

bool Board::is_empty(coordinates_t coordinates) const  {
    return coordinates.first < width && coordinates.second < height && fields[coordinates.first][coordinates.second] == EMPTY;
}