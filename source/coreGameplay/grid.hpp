#include "../etl/vector.h"

namespace coreGameplay
{
    etl::vector<etl::vector<uint16_t, 6>, 6> grid =
    {
        etl::vector<uint16_t, 6> { 0, 1, 2, 3, 4, 5 },
        etl::vector<uint16_t, 6> { 6, 7, 8, 9, 10, 11 },
        etl::vector<uint16_t, 6> { 12, 13, 14, 15, 16, 17 },
        etl::vector<uint16_t, 6> { 18, 19, 20, 21, 22, 23 },
        etl::vector<uint16_t, 6> { 24, 25, 26, 27, 28, 29 },
        etl::vector<uint16_t, 6> { 30, 31, 32, 33, 34, 35 }
    };
} // namespace coreGameplay