#include "gtest/gtest.h"

#include "math-utils.hpp"
#include <math.h>

TEST(LinearAlgebra, VectorLength2d)
{
    {
        Vector2d v = {1.0, 1.0};
        EXPECT_FLOAT_EQ(sqrt(2.0), vectorLength(v));
    }
    {
        Vector2d v = {0.0, 0.0};
        EXPECT_FLOAT_EQ(0.0, vectorLength(v));
    }
}

TEST(LinearAlgebra, VectorLength3d)
{
    {
        Vector3d v = {1.0, 1.0, 1.0};
        EXPECT_FLOAT_EQ(sqrt(3.0), normal(v));
    }
    {
        Vector3d v = {0.0, 0.0, 0.0};
        EXPECT_FLOAT_EQ(0.0, normal(v));
    }
}


TEST(LinearAlgebra, Normalisation)
{
    {
        Vector3d v = {1, 1, 1};
        normalize(v);
        EXPECT_FLOAT_EQ(1.0 / sqrt(3.0), v[0]);
        EXPECT_FLOAT_EQ(1.0 / sqrt(3.0), v[1]);
        EXPECT_FLOAT_EQ(1.0 / sqrt(3.0), v[2]);
    }
    /*
    {
        Vector3d v = {0, 0, 0};
        EXPECT_ANY_THROW(normalize(v));
    }
    */
}

TEST(LinearAlgebra, makeXPositive)
{ // void makeXPositive(Vector2d vec, const double tanDeltaAlpha = 0.05);
    {
        Vector2d v = {1.0, 1.0};
        makeXPositive(v);
        EXPECT_FLOAT_EQ(1.0, v[0]);
        EXPECT_FLOAT_EQ(1.0, v[1]);
    }
    {
        Vector2d v = {-1.0, 1.0};
        makeXPositive(v);
        EXPECT_FLOAT_EQ(1.0, v[0]);
        EXPECT_FLOAT_EQ(-1.0, v[1]);
    }
    {
        Vector2d v = {-1.0, -1.0};
        makeXPositive(v);
        EXPECT_FLOAT_EQ(1.0, v[0]);
        EXPECT_FLOAT_EQ(1.0, v[1]);
    }
    {
        Vector2d v = {0.0, 0.0};
        makeXPositive(v);
        EXPECT_FLOAT_EQ(0.0, v[0]);
        EXPECT_FLOAT_EQ(0.0, v[1]);
    }
    {
        Vector2d v = {-0.01, 1.0};
        makeXPositive(v);
        EXPECT_FLOAT_EQ(-0.01, v[0]);
        EXPECT_FLOAT_EQ(1.0, v[1]);
    }
}

TEST(LinearAlgebra, LinearRegressionFinder)
{
    {
        double x[] = {1.0,-1.0};
        double y[] = {0.0, 0.0};
        LinearRegression2DFinder finder;
        finder.setArrays(2, x, y);
        EXPECT_FLOAT_EQ(0, finder.getDistance());
        EXPECT_TRUE(checkIfEqual(finder.getAngle(), M_PI/2) || checkIfEqual(finder.getAngle(), -M_PI/2))
            << "Angle was " << finder.getAngle();
    }
    {
        double x[] = {1.0, 0.0};
        double y[] = {0.0, 1.0};
        LinearRegression2DFinder finder;
        finder.setArrays(2, x, y);
        EXPECT_NEAR(-sqrt(2.0)/2.0, finder.getDistance(), DOUBLE_PRECISION);
        EXPECT_TRUE(checkIfEqual(finder.getAngle(), M_PI/4.0) || checkIfEqual(finder.getAngle(), -3.0*M_PI/4.0))
            << "Angle was " << finder.getAngle();
    }
    {
        double x[] = { 1.0,-1.0, 1.0};
        double y[] = {-0.1, 0.0, 0.1};
        LinearRegression2DFinder finder;
        finder.setArrays(3, x, y);
        EXPECT_NEAR(0, finder.getDistance(), DOUBLE_PRECISION);
        EXPECT_TRUE(checkIfEqual(finder.getAngle(), M_PI/2) || checkIfEqual(finder.getAngle(), -M_PI/2))
            << "Angle was " << finder.getAngle();
    }
    {
        double x[] = {  1.0, -1.0, 1.0,-1.0,   1.0,  -1.0, 5.0,-5.0,  0.2, -0.2,   1.0,  -1.0};
        double y[] = { 20.0, 20.0, 0.0, 0.0, -30.0, -30.0, 2.0, 2.0, -7.0, -7.0, -10.0, -10.0};
        LinearRegression2DFinder finder;
        finder.setArrays(12, x, y);
        EXPECT_NEAR(0, finder.getDistance(), DOUBLE_PRECISION);
        EXPECT_TRUE(checkIfEqual(finder.getAngle(), 0) || checkIfEqual(finder.getAngle(), M_PI))
            << "Angle was " << finder.getAngle();
        EXPECT_NEAR(finder.getError(), 58.080, DOUBLE_PRECISION);
    }
    {   // The same points, but rotated by 0.2    
        double x[] = {  -2.9933200380599825e+00,  -4.9534531937424662e+00,  9.8006657784124163e-01,  -9.8006657784124163e-01,   6.9401465016930777e+00,   4.9800133460105949e+00,   4.5029942276160853e+00,  -5.2976715507963306e+00,   1.5866986311336768e+00, 1.1946719999971800e+00,   2.9667598857918538e+00,   1.0066267301093705e+00 };
        double y[] = {   1.9800000887619891e+01,   1.9402662226029772e+01,   1.9866933079506122e-01,  -1.9866933079506122e-01,  -2.9203328004442188e+01,  -2.9600666666032307e+01,   2.9534798096577894e+00,  9.6678650170717717e-01,  -6.8207321787296795e+00, -6.9001999110477037e+00,  -9.6019964476173545e+00,  -9.9993351092074771e+00 };
        LinearRegression2DFinder finder;
        finder.setArrays(12, x, y);
        EXPECT_NEAR(0, finder.getDistance(), DOUBLE_PRECISION);
        EXPECT_TRUE(checkIfEqual(finder.getAngle(), 0.2) || checkIfEqual(finder.getAngle(), -M_PI+0.2))
            << "Angle was " << finder.getAngle();
        EXPECT_NEAR(finder.getError(), 58.080, DOUBLE_PRECISION);
    }
}
