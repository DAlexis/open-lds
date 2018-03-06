#include "gtest/gtest.h"

#include "math-utils.hpp"
#include <memory>

struct Point2D
{
    Point2D(double _x = 0.0, double _y = 0.0) :
        x(_x), y(_y)
    {}
    
    double x, y;
    
    double operator-(const Point2D& arg)
    {
        return sqrt( sqr(x-arg.x) + sqr(y-arg.y) );
    }
};

Point2D weightMiddle(const Point2D& p1, double w1, const Point2D& p2, double w2)
{
    Point2D result;
    result.x = (p1.x*w1 + p2.x*w2)/(w1+w2);
    result.y = (p1.y*w1 + p2.y*w2)/(w1+w2);
    return result;
}

void createTwoCircles(double centerX1, double centerY1, double centerX2, double centerY2, std::shared_ptr<IClusterizer<Point2D>> pClusterzer)
{
    const int pointsCount = 10;
    
    for (int i=0; i<pointsCount; i++)
        pClusterzer->add(Point2D(centerX1 + 4*cos(2*M_PI*i/pointsCount), centerY1 + 4*sin(2*M_PI*i/pointsCount)));
    
    for (int i=0; i<pointsCount; i++)
        pClusterzer->add(Point2D(centerX2 + 3*cos(2*M_PI*i/pointsCount), centerY2 + 4*sin(2*M_PI*i/pointsCount)));
}

TEST(ClusterizatorCompact, TwoPoints1)
{
    std::shared_ptr<IClusterizer<Point2D>> pClusterzer(ClusterizatorsBuilder<Point2D>().createClusterizer("compact", weightMiddle));
    
    Point2D p1(0.0, 0.0), p2(10.0, 10.0);
    pClusterzer->add(p1);
    pClusterzer->add(p2);
    pClusterzer->clusterize(1.0);
    
    ASSERT_EQ(pClusterzer->clusters().size(), 2);
    
    EXPECT_NEAR(pClusterzer->clusters().front()->center().x, p1.x, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().front()->center().y, p1.y, DOUBLE_PRECISION);
    
    EXPECT_NEAR(pClusterzer->clusters().back()->center().x, p2.x, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().back()->center().y, p2.y, DOUBLE_PRECISION);
}

TEST(ClusterizatorCompact, TwoPoints2)
{
    std::shared_ptr<IClusterizer<Point2D>> pClusterzer(ClusterizatorsBuilder<Point2D>().createClusterizer("compact", weightMiddle));
    
    Point2D p1(0.0, 0.0), p2(10.0, 10.0);
    pClusterzer->add(p1);
    pClusterzer->add(p2);
    pClusterzer->clusterize(15.0);
    
    ASSERT_EQ(pClusterzer->clusters().size(), 1);
    
    EXPECT_NEAR(pClusterzer->clusters().front()->center().x, 5.0, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().front()->center().y, 5.0, DOUBLE_PRECISION);
}

TEST(ClusterizatorCompact, TwoCircles1)
{
    std::shared_ptr<IClusterizer<Point2D>> pClusterzer(ClusterizatorsBuilder<Point2D>().createClusterizer("compact", weightMiddle));
    
    const double centerX1 = 0;
    const double centerY1 = 0;
    const double centerX2 = 25;
    const double centerY2 = 20;
    
    createTwoCircles(centerX1, centerY1, centerX2, centerY2, pClusterzer);
    
    pClusterzer->clusterize(8.5);
    
    ASSERT_EQ(pClusterzer->clusters().size(), 2);
    
    EXPECT_NEAR(pClusterzer->clusters().front()->center().x, centerX1, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().front()->center().y, centerY1, DOUBLE_PRECISION);
    
    EXPECT_NEAR(pClusterzer->clusters().back()->center().x, centerX2, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().back()->center().y, centerY2, DOUBLE_PRECISION);
}

TEST(ClusterizatorCompactQuick, TwoPoints1)
{
    std::shared_ptr<IClusterizer<Point2D>> pClusterzer(ClusterizatorsBuilder<Point2D>().createClusterizer("compact-quick", weightMiddle));
    
    Point2D p1(0.0, 0.0), p2(10.0, 10.0);
    pClusterzer->add(p1);
    pClusterzer->add(p2);
    pClusterzer->clusterize(1.0);
    
    ASSERT_EQ(pClusterzer->clusters().size(), 2);
    
    EXPECT_NEAR(pClusterzer->clusters().front()->center().x, p1.x, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().front()->center().y, p1.y, DOUBLE_PRECISION);
    
    EXPECT_NEAR(pClusterzer->clusters().back()->center().x, p2.x, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().back()->center().y, p2.y, DOUBLE_PRECISION);
}

TEST(ClusterizatorCompactQuick, TwoCircles1)
{
    std::shared_ptr<IClusterizer<Point2D>> pClusterzer(ClusterizatorsBuilder<Point2D>().createClusterizer("compact-quick", weightMiddle));
    
    const double centerX1 = 0;
    const double centerY1 = 0;
    const double centerX2 = 25;
    const double centerY2 = 20;
    
    createTwoCircles(centerX1, centerY1, centerX2, centerY2, pClusterzer);
    
    pClusterzer->clusterize(8.5);
    
    ASSERT_EQ(pClusterzer->clusters().size(), 2);
    
    EXPECT_NEAR(pClusterzer->clusters().front()->center().x, centerX1, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().front()->center().y, centerY1, DOUBLE_PRECISION);
    
    EXPECT_NEAR(pClusterzer->clusters().back()->center().x, centerX2, DOUBLE_PRECISION);
    EXPECT_NEAR(pClusterzer->clusters().back()->center().y, centerY2, DOUBLE_PRECISION);
}
