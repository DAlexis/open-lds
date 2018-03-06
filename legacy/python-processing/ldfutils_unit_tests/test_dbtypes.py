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

if __name__ == "__main__":
    unittest.main()
