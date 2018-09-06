import ldfutils.pos_time
import ldfutils.utils as ut

import numpy as np
import math
import array
import copy


class LightningMark:
    def __init__(self, **kwargs):
        self.intensity_info = None
        self.time = ldfutils.pos_time.PrecisionTime.from_dict(kwargs)
        # if 'precision_time' in kwargs:
        #     if not isinstance(kwargs['precision_time'], ldfutils.pos_time.PrecisionTime):
        #         raise ValueError("precision_time must be instance of PrecisionTime")
        #     self.time = copy.copy(kwargs['precision_time'])
        # elif 'str_time' in kwargs:
        #     self.time = ldfutils.pos_time.PrecisionTime.from_string(kwargs['str_time'])
        # elif 'round_time' in kwargs and 'fractional_time' in kwargs:
        #     self.time = ldfutils.pos_time.PrecisionTime(
        #         round_datetime=kwargs['round_time'],
        #         fractional_time=kwargs['fractional_time']
        #     )
        # else:
        #     raise ValueError("LightningMark cannot be instantiated without date and time")

        if 'lat' in kwargs and 'lon' in kwargs:
            self.pos = ldfutils.pos_time.Position(lat=kwargs['lat'], lon=kwargs['lon'])
        else:
            if 'pos' not in kwargs:
                raise ValueError("LightningMark cannot be instantiated without position")
            if not isinstance(kwargs['pos'], ldfutils.pos_time.Position):
                raise ValueError("pos must be an instance of Position")
            self.pos = copy.copy(kwargs['pos'])

        if 'time_cluster' not in kwargs:
            raise ValueError("time_cluster is not set")
        self.time_cluster = kwargs['time_cluster']

        if 'id' in kwargs:
            self.id = kwargs['id']
        else:
            self.id = 0
        self.time_since_group_begin = 0.0

    def add_intensity_info(self, intensity_info):
        self.intensity_info = intensity_info

    def __str__(self):
        return '[Lightning mark] id=' + str(self.id) + \
               ' pos=' + str(self.pos) + \
               ' time="' + str(self.time) + '"'\
               ' time_since_group_begin=' + str(self.time_since_group_begin)

    def __repr__(self):
        return self.__str__()


class TimeCluster:
    def __init__(self, **kwargs):
        self.id = kwargs["id"]
        self.time = ldfutils.pos_time.PrecisionTime.from_dict(kwargs)
        self.strikes_count = kwargs["strikes_count"]
        strikes_int = ut.split_ids_from_str(kwargs["strikes"])
        self.strikes = list(filter(None, strikes_int))
        if self.strikes_count != len(self.strikes):
            raise ValueError("Cannot initialize TimeCluster when strikes_count = " + str(self.strikes_count) +
                             " that is not equals actually size of " + str(self.strikes))

    def __str__(self):
        return '[TimeCluster] id=' + str(self.id) + \
               ' strikes_count=' + str(self.strikes_count) + \
               ' time="' + str(self.time) + '"'\
               ' strikes=' + str(self.strikes)

    def __repr__(self):
        return self.__str__()


class Strike:
    def __init__(self, **kwargs):
        """
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `device_id` int(11) NOT NULL,
          `experiment_id` int(11) NOT NULL,
          `round_time` datetime NOT NULL,
          `fraction_time` double NOT NULL,
          `buffer_duration` double NOT NULL,
          `buffer_values_count` int(11) NOT NULL,
          `buffer_begin_time_shift` double NOT NULL,
          `lon` double NOT NULL,
          `lat` double NOT NULL,
          `direction` double,
          `direction_error` double,
          `E_field` blob NOT NULL,
          `MN_field` blob NOT NULL,
          `MW_field` blob NOT NULL,
        """
        self.id = kwargs["id"]
        self.device_id = kwargs["device_id"]
        if "experiment_id" in kwargs:
            self.experiment_id = kwargs["experiment_id"]
        else:
            self.experiment_id = 0
        self.time = ldfutils.pos_time.PrecisionTime.from_dict(kwargs)
        self.position = ldfutils.pos_time.Position.from_dict(kwargs)
        self.e_field = list(array.array('d', kwargs["E_field"]))
        self.mn_field = list(array.array('d', kwargs["MN_field"]))
        self.mw_field = list(array.array('d', kwargs["MW_field"]))

    def __str__(self):
        return '[Strike] id=' + str(self.id) + \
               ' device_id=' + str(self.device_id) + \
               ' time="' + str(self.time) + '"'\
               ' pos=' + str(self.position)

    def __repr__(self):
        return self.__str__()


class FullStrikeInfo:
    def __init__(self, id, mark, strikes):
        if not isinstance(mark, LightningMark):
            raise ValueError("mark argument should be a LightningMark")

        if not isinstance(strikes, list):
            raise ValueError("strikes should be a list")

        if not len(strikes) != 0:
            raise ValueError("strikes should not be empty")

        if not isinstance(strikes[0], Strike):
            raise ValueError("strikes should be a list of Strike's")

        self.id = id
        self.strikes = strikes
        self.mark = mark

    def __str__(self):
        return '[FullStrikeInfo] id=' + str(self.id) + \
               ' strikes=' + str(self.strikes) + \
               ' mark=(' + str(self.mark) + ')'

    def __repr__(self):
        return self.__str__()


class IntensityInfo:
    intensity_id_just_created = -1

    def __init__(self, solution_id, intensities, min_dist, intensity_id=intensity_id_just_created):
        self.intensity_id = intensity_id
        self.solution_id = solution_id
        self.intensities = list(intensities)
        self.intensity = np.mean(self.intensities)
        self.min_dist = min_dist
        self.mean_square_normed = math.sqrt(np.square(self.intensities - self.intensity).mean()) / self.intensity

    def __str__(self):
        return '[IntensityInfo] solution_id=' + str(self.solution_id)

    def intensities_str(self):
        result = ""
        for i in self.intensities:
            result += str(i) + ", "
        return result

    def __repr__(self):
        return self.__str__()


class RepetitionGroup:
    group_id_just_created = -1

    def __init__(self, group_id=group_id_just_created, solutions=None):
        self.id = group_id
        if solutions is not None:
            self.solutions = solutions  # type: list[LightningMark]
        else:
            self.solutions = []  # type: list[LightningMark]

    def add(self, solution):
        self.solutions.append(solution)

    def last(self):
        return self.solutions[-1]

    def sort(self):
        self.solutions.sort(key=lambda x: x.time)

    def solutions_string(self):
        result = ""
        for sol in self.solutions:
            result += str(sol.id) + ", "
        return result

    def time(self):
        return self.solutions[0].time

    def __str__(self):
        return "[RepetitionGroup] solutions: " + str(self.solutions)

    def __repr__(self):
        return self.__str__()

    def __len__(self):
        return len(self.solutions)
