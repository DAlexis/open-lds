import ldfutils.connection
import ldfutils.dbtypes as dbt
import ldfutils.utils as ut
from typing import Dict, Tuple, List


def load_solutions(cursor, conditions="1"):
    def get_lightning_mark_from_cursor(row):
        (id, round_time, fraction_time, lon, lat, time_cluster) = row
        return dbt.LightningMark(id=id, round_time=round_time, fractional_time=fraction_time, lat=lat, lon=lon, time_cluster=time_cluster)

    query = ('SELECT id, round_time, fraction_time, lon, lat, time_cluster FROM solutions '
             'WHERE ' + conditions)
    cursor.execute(query)

    if cursor.rowcount == 0:
        return []

    lightnings = [get_lightning_mark_from_cursor(cursor.next())]
    for row in cursor:
        next_lightning = get_lightning_mark_from_cursor(row)
        next_lightning.time_since_group_begin = next_lightning.time - lightnings[0].time
        lightnings.append(next_lightning)

    return lightnings


def load_time_clusters(cursor, conditions="1"):
    def parse_row(row):
        (cluster_id, round_time, fraction_time, strikes_count, strikes) = row
        return dbt.TimeCluster(
            id=cluster_id,
            round_time=round_time,
            fraction_time=fraction_time,
            strikes_count=strikes_count,
            strikes=strikes
        )

    query = ('SELECT id, round_time, fraction_time, strikes_count, strikes FROM time_clusters '
             'WHERE ' + conditions)

    cursor.execute(query)

    clusters = [parse_row(row) for row in cursor]
    return clusters


def load_strikes(cursor, conditions="0"):
    def parse_row(row):
        (id, device_id, experiment_id, E_field, MN_field, MW_field, round_time, fraction_time, lat, lon) = row
        return dbt.Strike(
            id=id,
            device_id=device_id,
            experiment_id=experiment_id,
            E_field=E_field,
            MN_field=MN_field,
            MW_field=MW_field,
            round_time=round_time,
            fraction_time=fraction_time,
            lat=lat,
            lon=lon
        )

    query = ('SELECT id, device_id, experiment_id, E_field, MN_field, MW_field, round_time, fraction_time, lat, lon '
             'FROM unified_strikes WHERE ' + conditions)

    cursor.execute(query)

    strikes = [parse_row(row) for row in cursor]
    return strikes


def load_solutions_details(cursor, solutions):
    full_solutions = {}
    for solution in solutions:
        full_solutions[solution.id] = {}
        current_full_solution = full_solutions[solution.id]
        current_full_solution["solution"] = solution
        time_cluster = load_time_clusters(cursor, "id = " + str(solution.time_cluster))[0]
        strikes = []

        for i in time_cluster.strikes:
            strikes.append(load_strikes(cursor, conditions="id=" + str(i))[0])

        full_solutions[solution.id] = dbt.FullStrikeInfo(solution.id, solution, strikes)
    return full_solutions


def load_full_solutions(cursor, solution_conditions=""):
    solutions = load_solutions(cursor, solution_conditions)
    return load_solutions_details(cursor, solutions)


def is_intensity_info_exists(cursor, solution_id):
    query = 'SELECT id FROM intensity WHERE solutions_id = ' + str(solution_id)
    cursor.execute(query)
    return cursor.rowcount != 0


def add_time_limits_to_option_parser(option_parser, default_from="2015-06-01 00:00:00", default_to="2015-08-01 00:00:00"):
    option_parser.add_option("-b", "--begin", dest="begin_time", default=default_from,
                               help="Start time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

    option_parser.add_option("-e", "--end", dest="end_time", default=default_to,
                               help="End time for extracting", metavar="yyyy-mm-dd hh:mm:ss")
    return option_parser


def load_repetition_groups(cursor, conditions="0"):
    def parse_row(row):
        (group_id, solutions_str) = row
        solutions = ut.split_ids_from_str(solutions_str)
        group = dbt.RepetitionGroup(group_id)
        for sol in solutions:
            group.add(load_solutions(cursor, conditions="id = " + str(sol))[0])
        return group

    query = ('SELECT id, solutions '
             'FROM rep_strokes WHERE ' + conditions)

    cursor.execute(query)

    # First reading all rows
    all_rows = [row for row in cursor]

    # Then parsing it and selecting solutions
    groups = [parse_row(row) for row in all_rows]
    return groups


def load_intensity_info_for_repetition_groups(cursor, rep_groups: List[dbt.RepetitionGroup]):
    for rg in rep_groups:
        for sol in rg.solutions:
            sol.add_intensity_info(load_intensity_info(cursor, sol.id))
    return rep_groups


def load_intensity_info(cursor, solution_id):
    def parse_row(row):
        (intensity_id, solutions_id, intensities, min_distance) = row
        return dbt.IntensityInfo(solution_id=solution_id, intensities=ut.split_floats_from_str(intensities), min_dist=min_distance, intensity_id=intensity_id)

    query = ('SELECT id, solutions_id, intensities, min_distance'
             ' FROM intensity WHERE solutions_id=' + str(solution_id))

    cursor.execute(query)

    row = next(cursor, None)
    if row is not None:
        return parse_row(row)
    else:
        return None

