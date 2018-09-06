import ldfutils.pos_time as pt


class ConditionGenerator:
    @staticmethod
    def round_time_interval(time_from, time_to):
        return '(`round_time` BETWEEN "{}" AND "{}")'.format(time_from, time_to)

    @staticmethod
    def not_on_the_farest():
        return "status = 0"

    @staticmethod
    def in_square(lat1, lon1, lat2, lon2):
        if lat2 < lat1:
            lat2, lat1 = lat2, lat2
        if lon2 < lon1:
            lon2, lon1 = lon1, lon2
        return "(lat>={} && lon>={} && lat<{} && lon<{})".format(lat1, lon1, lat2, lon2)


def month_interval(begin=0, end=1):
    months = [
        "2014-05-01 00:00:00",
        "2014-06-01 00:00:00",
        "2014-07-01 00:00:00",
        "2014-08-01 00:00:00",
        "2014-09-01 00:00:00",

        "2015-05-01 00:00:00",
        "2015-06-01 00:00:00",
        "2015-07-01 00:00:00",
        "2015-08-01 00:00:00",
        "2015-09-01 00:00:00",

        "2016-05-01 00:00:00",
        "2016-06-01 00:00:00",
        "2016-07-01 00:00:00",
        "2016-08-01 00:00:00",
        "2016-09-01 00:00:00",

        "2017-05-01 00:00:00",
        "2017-06-01 00:00:00",
        "2017-07-01 00:00:00",
        "2017-08-01 00:00:00",
        "2017-09-01 00:00:00",

        "2018-05-01 00:00:00",
        "2018-06-01 00:00:00",
        "2018-07-01 00:00:00",
        "2018-08-01 00:00:00",
        "2018-09-01 00:00:00",
    ]
    return ConditionGenerator.round_time_interval(months[begin], months[end])


def split_ids_from_str(str_ids):
    ids = [int(x) for x in filter(None, str_ids.replace(" ", "").split(','))]
    return ids


def split_floats_from_str(str_ids):
    ids = [float(x) for x in filter(None, str_ids.replace(" ", "").split(','))]
    return ids


def time_decomposition(time_from: pt.PrecisionTime, time_to: pt.PrecisionTime, step: float, callable, **kwargs):
    """
    Call function with begin_time, end_time arguments for given time period with given step
    :param time_from:
    :param time_to:
    :param step:
    :param callable: function to call
    :param kwargs:
    :return:
    """
    chunk_begin_time = time_from
    chunk_end_time = chunk_begin_time + step

    while chunk_end_time < time_to:
        callable(**kwargs, begin_time=chunk_begin_time, end_time=chunk_end_time)
        chunk_begin_time = chunk_end_time
        chunk_end_time += step
