import ldfutils.dbtypes as lm
import numpy as np


def source_energy(energy, dist):
    return energy * (dist ** 2)


def strike_energy(strike: lm.Strike):
    return np.amax(np.square(strike.mn_field) + np.square(strike.mw_field))


def solution_energy(sol: lm.FullStrikeInfo):
    ens = []
    dists = []
    for strike in sol.strikes:
        dist = strike.position - sol.mark.pos
        dists.append(dist)
        en = strike_energy(strike)
        src_en = source_energy(en, dist)
        #print("id = ", strike.id, "dev_id=", strike.device_id, " dist=", dist, " en=", en, "src_en=", src_en)
        ens.append(src_en)
    return np.average(ens), ens, dists


def get_repeated_strikes_intervals(lightnings: lm.LightningMark, repeated_interval_max, repeated_dist_max):
    """
    Get intervals between strikes, devides by their repetitions count. Repetition criteria consists of
    repeated_interval_max and repeated_dist_max
    :param lightnings: array of LightningMarks. Will be sorted by time!
    :param repeated_interval_max: Maximal time interval between lightnings to consider their repeated
    :param repeated_dist_max: Maximal distance betwiin lightnings to consider their repeated
    :return: array of arrays of intervals. intervals[2][0] - first registered interval for repetitions count = 2
    """
    lightnings.sort(key=lambda x: x.time)

    intervals = [[]]
    lightnings_vs_repetition = [[]]
    strokes_count = 0
    prev = lightnings[0]
    for current in lightnings[1:]:
        dt = current.time - prev.time
        dist = current.pos - prev.pos
        if dt < repeated_interval_max and dist < repeated_dist_max:
            # current is a repetition of prev
            strokes_count += 1
            if len(intervals) == strokes_count:
                intervals.append([])
                lightnings_vs_repetition.append([])
        else:
            # current is single
            strokes_count = 0
        intervals[strokes_count].append(dt)
        lightnings_vs_repetition[strokes_count].append(current)
        prev = current

    return intervals, lightnings_vs_repetition


def repetition_conditional_probability(intervals, max_repetitions_count=0):
    if max_repetitions_count == 0:
        max_repetitions_count = len(intervals) - 1
    repetitions_count = [len(intervals[i]) for i in range(0, max_repetitions_count + 1)]
    probabilities = [1]  # Probability of zero repetition
    for rep in range(1, max_repetitions_count + 1):
        probabilities.append(float(repetitions_count[rep]) / repetitions_count[rep - 1])

    return repetitions_count, probabilities
