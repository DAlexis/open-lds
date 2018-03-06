#include "gtest/gtest.h"

#include "dfmath.hpp"
#include <cmath>

#include <string>

TEST(Positioning, TimeStringification)
{
    {
        Time time1;
        time1.setCurrent();
        std::string str = time1.getString();
        Time time2;
        time2.setTime(str);
        EXPECT_EQ(time1, time2);
        EXPECT_EQ(time1.getString(), time2.getString());
    }
}

TEST(Positioning, CurveFinding)
{
    {
        Position pos1(0,0);
        Position pos2(90,0);
        EXPECT_NEAR(M_PI_2, pos1.curve(pos2), DOUBLE_PRECISION);
    }
    {
        Position pos1(0,0);
        Position pos2(0,90);
        EXPECT_NEAR(M_PI_2, pos1.curve(pos2), DOUBLE_PRECISION);
    }
    {
        Position pos1(0,-90);
        Position pos2(0,90);
        EXPECT_NEAR(M_PI, pos1.curve(pos2), DOUBLE_PRECISION);
    }
}

TEST(Positioning, Azimuth)
{
    { // One day this data coused nan, so I've added test here
        Position pos1(56.686904444444437, 43.432369999999999);
        Position pos2(56.330942038888892, 43.432369999999999);
        double result = pos1.getAzimuth(pos2);
        EXPECT_FALSE(std::isnan(result));
    }
    {
        Position pos1(56.0, 44.0);
        Position pos2(56.5, 44.5);
        Position pos3(56.5, 43.5);
        EXPECT_GT(angleTo14Quadrant(pos1.getAzimuth(pos2)), 0);
        EXPECT_LT(angleTo14Quadrant(pos1.getAzimuth(pos3)), 0);
    }
}
