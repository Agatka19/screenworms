#include "random_generator.h"

using namespace records;

RandomNumberGenerator::RandomNumberGenerator(rand_t seed)  : last_rand(seed) {

}

rand_t RandomNumberGenerator::next()  {
    rand_t rand = last_rand;
    last_rand = static_cast<rand_t>((last_rand * RANDOM_MUL) % RANDOM_MOD);
    return rand;
}
