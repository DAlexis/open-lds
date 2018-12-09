import ldfutils.connection
import ldfutils.dbtypes as dbt

# TODO: Add row alteration support via if id != id_just_created


def write_intensity_info(cursor, info: dbt.IntensityInfo, fake=False):
    query = ('INSERT INTO intensity(`solutions_id`, `intensity`, `mean_square_normed`, `intensities`, `min_distance`)'
             ' VALUES (' + str(info.solution_id) +
             ', ' + str(info.intensity) +
             ', ' + str(info.mean_square_normed) +
             ', "' + info.intensities_str() +
             '", ' + str(info.min_dist) + ')')
    if fake:
        print(query)
    else:
        cursor.execute(query)


def write_repetition_group(cursor, group: dbt.RepetitionGroup, fake=False):
    query = ('INSERT INTO rep_strokes(`round_time`, `fraction_time`, `solutions_count`, `solutions`)'
             ' VALUES ("' + str(group.time().round_time()) +
             '", ' + str(group.time().fractional_time()) +
             ', ' + str(len(group)) +
             ', "' + group.solutions_string() +
             '")')

    if fake:
        print(query)
    else:
        cursor.execute(query)

