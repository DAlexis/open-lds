#include "gtest/gtest.h"

#include "dfmath.hpp"
#include <cmath>

TEST(TimeShift, EmptyFieldBuffer)
{
    //double getTimeShift(const StrikeData& first, const StrikeData& second, double precision = 0);
    StrikeData s1;
    StrikeData s2;
    ASSERT_EQ(getTimeShift(s1, s2, 1e-3), 0.0);
}
