#ifndef SIK_records_RANDOM_H
#define SIK_records_RANDOM_H

#include "types.h"

using namespace std;

namespace records {
    constexpr uint64_t RANDOM_MUL = 279410273;
    constexpr uint64_t RANDOM_MOD = 4294967291;

    class RandomNumberGenerator {
    private:
        rand_t last_rand;
    public:
        RandomNumberGenerator(rand_t seed) ;
        rand_t next() ;
    };
}


#endif
