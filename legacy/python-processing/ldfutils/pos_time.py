import datetime
import re
import copy
import math

EarthRadius = 6371000


def curve_to_distance(curve):
    return curve / 180 * math.pi * EarthRadius


def sqr(a):
    return a*a

# def generate_time_conditions(roundTimeFrom, fractionTimeFrom, roundTimeTo, fractionTimeTo):
#     condition = ("(round_time>'{0}' || (round_time='{0}' && fraction_time>{1})) "
#         "&& (round_time<'{2}' || (round_time='{2}' && fraction_time<{3}))")
#     return condition.format(roundTimeFrom, fractionTimeFrom, roundTimeTo, fractionTimeTo)


def haversine(angle):
    return sqr(math.sin(angle/2.0))


def inverse_haversine(value):
    return 2.0*math.asin(math.sqrt(value))


def distance(lat1, lon1, lat2, lon2):
    """
    Calculate distance using haversine formulae
    :param lat1: Latitude of point 1
    :param lon1: Longitude of point 1
    :param lat2: Latitude of point 2
    :param lon2: Longitude of point 2
    :return: distance in meters
    """
    theta = (lon2-lon1) * math.pi / 180
    phi1 = lat1 * math.pi / 180
    phi2 = lat2 * math.pi / 180
    curve = inverse_haversine(haversine(phi1-phi2) + math.cos(phi1)*math.cos(phi2)*haversine(theta))
    return curve * 6371000


class PrecisionTime:
    default_round_time_format = "%Y-%m-%d %H:%M:%S"

    @staticmethod
    def from_string(string):
        """
        Convert from string
        :param string: argument in format "2015-06-26 03:40:00.123"
        :return: PrecisionTime object
        """
        rf = string.split('.', 1)
        if len(rf) == 1:
            return PrecisionTime(rf[0], 0.0)
        else:
            return PrecisionTime(rf[0], float("0." + rf[1]))

    @staticmethod
    def from_dict(kwargs):
        if 'precision_time' in kwargs:
            if not isinstance(kwargs['precision_time'], PrecisionTime):
                raise ValueError("precision_time must be instance of PrecisionTime")
            time = copy.copy(kwargs['precision_time'])
        elif 'str_time' in kwargs:
            time = PrecisionTime.from_string(kwargs['str_time'])
        elif 'round_time' in kwargs and 'fractional_time' in kwargs:
            time = PrecisionTime(
                round_datetime=kwargs['round_time'],
                fractional_time=kwargs['fractional_time']
            )
        elif 'round_time' in kwargs and 'fraction_time' in kwargs:
            time = PrecisionTime(
                round_datetime=kwargs['round_time'],
                fractional_time=kwargs['fraction_time']
            )
        else:
            raise ValueError("Precision time not avaliable")

        return time

    def __init__(self, round_datetime, fractional_time, round_format=default_round_time_format):
        if isinstance(round_datetime, datetime.datetime):
            self.round = copy.copy(round_datetime)
        else:
            self.round = datetime.datetime.strptime(round_datetime, round_format)
        self.fractional = float(fractional_time)

    def round_time(self):
        return str(self.round)

    def fractional_time(self):
        return self.fractional

    def __add__(self, seconds):
        result = copy.copy(self)
        result.fractional += seconds
        round_secs = int(result.fractional)
        if result.fractional < 0.0 and result.fractional != round_secs:
            round_secs -= 1

        result.fractional -= round_secs
        result.round += datetime.timedelta(seconds=round_secs)
        return result

    def __sub__(self, other):
        if isinstance(other, PrecisionTime):
            dt = self.round - other.round
            return dt.total_seconds() + self.fractional - other.fractional
        else:
            return self+(-other)

    def __lt__(self, other):
        if self.round == other.round:
            return self.fractional < other.fractional
        else:
            return self.round < other.round

    def __gt__(self, other):
        return other < self

    def __ge__(self, other):
        return not self < other

    def __le__(self, other):
        return not other < self

    def __str__(self):
        return str(self.round) + str(self.fractional)[1:]

    def __repr__(self):
        return self.__str__()


class Position:
    @staticmethod
    def from_dict(d):
        return Position(lat=d["lat"], lon=d["lon"])

    def __init__(self, lat, lon):
        self.lat = lat
        self.lon = lon

    def __sub__(self, right):
        return distance(self.lat, self.lon, right.lat, right.lon)

    def __str__(self):
        return '(' + str(self.lat) + '; ' + str(self.lon) + ')'

    def __repr__(self):
        return self.__str__()

    def NS(self):
        if self.lat >= 0:
            return 'N'
        else:
            return 'S'

    def EW(self):
        if self.lat >= 0:
            return 'E'
        else:
            return 'W'


def make_datetime(string_round, string_fractional):
    parts = re.split('-|:| ', string_round)
    return datetime.datetime(int(parts[0]),  # Year
                    int(parts[1]),  # Month
                    int(parts[2]),  # Day
                    int(parts[3]),  # Hours
                    int(parts[4]),  # Minutes
                    int(parts[5]),  # Seconds
                    int(float(string_fractional)*1e6))  # Microseconds
