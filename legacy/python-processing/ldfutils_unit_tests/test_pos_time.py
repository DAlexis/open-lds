#!/usr/bin/env python3

import ldfutils.pos_time

import unittest
import datetime


class TestPrecisionTime(unittest.TestCase):
    def test_creation_datetime1(self):
        r = "2015-06-26 03:40:00"
        f = ".123"
        test_time = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        self.assertEqual(str(test_time), r+f)
        self.assertEqual(test_time.round_time(), r)
        self.assertAlmostEqual(test_time.fractional_time(), float("0" + f), 8)

    def test_creation_datetime2(self):
        r = "2015-06-26 03:40:00"
        f = ".0"
        test_time = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        self.assertEqual(str(test_time), r+f)
        self.assertEqual(test_time.round_time(), r)
        self.assertAlmostEqual(test_time.fractional_time(), float("0" + f), 8)

    def test_creation_from_string(self):
        r = "2015-06-26 03:40:00"
        f = ".123"
        test_time1 = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        test_time2 = ldfutils.pos_time.PrecisionTime.from_string(r+f)

        self.assertEqual(str(test_time1), str(test_time2))

    def test_creation_from_string2(self):
        r = "2015-06-26 03:40:00"
        f = ".0"
        test_time1 = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        test_time2 = ldfutils.pos_time.PrecisionTime.from_string(r)

        self.assertEqual(str(test_time1), str(test_time2))

    def test_creation_from_dict_fractional_time(self):
        r = "2015-06-26 03:40:00"
        f = ".123"
        test_time1 = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        test_time2 = ldfutils.pos_time.PrecisionTime.from_dict(
            {
                "round_time": r,
                "fractional_time": f
            }
        )

        self.assertEqual(str(test_time1), str(test_time2))

    def test_creation_from_dict_fraction_time(self):
        r = "2015-06-26 03:40:00"
        f = ".123"
        test_time1 = ldfutils.pos_time.PrecisionTime(
            datetime.datetime.strptime(r, ldfutils.pos_time.PrecisionTime.default_round_time_format),
            f
        )
        test_time2 = ldfutils.pos_time.PrecisionTime.from_dict(
            {
                "round_time": r,
                "fraction_time": f
            }
        )

        self.assertEqual(str(test_time1), str(test_time2))

    def test_creation(self):
        r = "2015-06-26 03:40:00"
        f = ".123"
        test_time = ldfutils.pos_time.PrecisionTime(r, f)
        self.assertEqual(str(test_time), r+f)

    def test_add(self):
        r = "2015-06-26 03:40:00"
        r2 = "2015-06-26 03:40:02"
        f = ".2"
        test_time = ldfutils.pos_time.PrecisionTime(r, f)
        test_time += 1.9
        self.assertEqual(test_time.round,
                         datetime.datetime.strptime(r2,
                                                    ldfutils.pos_time.PrecisionTime.default_round_time_format
                                                    )
                         )
        self.assertAlmostEqual(test_time.fractional, 0.1)

    def test_sub_time(self):
        t1 = ldfutils.pos_time.PrecisionTime("2015-06-26 03:40:00", "0.2")
        t2 = ldfutils.pos_time.PrecisionTime("2015-06-26 03:35:00", "0.5")
        self.assertAlmostEqual(t1-t2, 299.7)


class TestTimeConversion(unittest.TestCase):
    def test_make_datetime(self):
        sample = "2015-06-26 03:40:00"
        dt = ldfutils.pos_time.make_datetime("2015-06-26 03:40:00", "0.123456")


if __name__ == "__main__":
    unittest.main()
