#!/usr/bin/env python3

import ldfutils.dbtypes

import unittest
import datetime


class TestTimeCluster(unittest.TestCase):
    def test_creation(self):
        tc = ldfutils.dbtypes.TimeCluster(
            id=1,
            round_time="2015-06-26 03:40:00",
            fraction_time=".123",
            strikes_count=3,
            strikes="12,23,45  , "
        )
        self.assertEqual(len(tc.strikes), 3)
        self.assertTrue(isinstance(tc.strikes[0], int))
        self.assertTrue(isinstance(tc.strikes[1], int))
        self.assertTrue(isinstance(tc.strikes[2], int))

    def test_raise_on_creation(self):
        self.assertRaises(
            ValueError,
            ldfutils.dbtypes.TimeCluster,
            id=1,
            round_time="2015-06-26 03:40:00",
            fraction_time=".123",
            strikes_count=4,
            strikes="12,23,45  , "
        )


class TestIntensityInfo(unittest.TestCase):
    def test_creation(self):
        ii1 = ldfutils.dbtypes.IntensityInfo(solution_id=25, intensities=[3.0, 3.0, 3.0], min_dist=1000.0)
        self.assertEqual(ii1.solution_id, 25)
        self.assertEqual(ii1.intensity, 3.0)
        self.assertEqual(ii1.mean_square_normed, 0.0)
        self.assertEqual(ii1.min_dist, 1000.0)
        self.assertEqual(ii1.intensities_str(), "3.0, 3.0, 3.0, ")

        ii2 = ldfutils.dbtypes.IntensityInfo(solution_id=44, intensities=[1.0, 4.0], min_dist=1000.0)
        self.assertAlmostEqual(0.59999999999999998, ii2.mean_square_normed, places=8)

if __name__ == "__main__":
    unittest.main()
