#ifndef SIK_records_BOARD_H
#define SIK_records_BOARD_H


#include <vector>
#include "types.h"

using namespace std;

namespace records {
    class Board {
    private:
        /****************************
        *        board params
        ****************************/
        pixel_t width;
        pixel_t height;
        vector<vector<bool>> fields;

    public:
        Board(pixel_t width, pixel_t height) ;
        void clear() ;
        void eat(coordinates_t coordinates) ;
        bool is_empty(coordinates_t coordinates) const ;
    };
}


#endif
