#include "gtest/gtest.h"

#include "math-utils.hpp"

class GDSTest : public ::testing::Test
{
public:
    static double sqr(double x, double y)
    {
        return x*x+y*y;
    }
};

TEST_F(GDSTest, FucntionParabaloid)
{
    Function2D func = GDSTest::sqr;
    {
        GradientDescentSolver solver;
        solver.setStartPosition(10.0, 7.0);
        solver.setFunction(func);
        double defaultPrecision = 1e-7;
        // Using default precision
        // Using default start step
        solver.runVariableStep();
        EXPECT_GT( 10*defaultPrecision, fabs(solver.getX()) );
        EXPECT_GT( 10*defaultPrecision, fabs(solver.getY()) );
    }
    {
        GradientDescentSolver solver;
        solver.setStartPosition(0.0, 0.0);
        solver.setFunction(func);
        double precision = 1e-3;
        solver.setXYPrecision(precision);
        // Using default precision
        // Using default start step
        solver.runVariableStep();
        EXPECT_GT( 10*precision, fabs(solver.getX()) );
        EXPECT_GT( 10*precision, fabs(solver.getY()) );
    }
}
