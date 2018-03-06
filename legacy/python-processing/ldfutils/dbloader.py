from Tools.scripts.serve import app

import ldfutils.connection
import ldfutils.dbtypes as dbt


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
