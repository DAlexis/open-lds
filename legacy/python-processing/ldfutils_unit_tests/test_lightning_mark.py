#!/usr/bin/env python3

import ldfutils.dbtypes as lm

import unittest


class TestLightningMark(unittest.TestCase):
    def test_instantiation(self):
        self.assertRaises(ValueError, lm.LightningMark)
        self.assertRaises(ValueError, lm.LightningMark, str_time="2015-06-26 03:40:00.123", lat=1.2, lon=3.4)
        lm.LightningMark(str_time="2015-06-26 03:40:00.123", lat=1.2, lon=3.4, time_cluster=21)

if __name__ == "__main__":
    unittest.main()
