/*
 * positioning-tests.cpp
 *
 *  Created on: 22 апр. 2016 г.
 *      Author: dalexies
 */

#include "gtest/gtest.h"

#include "positioning.hpp"
#include <math.h>
#include <thread>

TEST(Time, FractionSetCurrent)
{
	Time t1;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	Time t2;
	Time t3;
	Time t4;
	ASSERT_GT(t2, t1);
	ASSERT_GT(t3, t2);
	ASSERT_GT(t4, t3);
}
