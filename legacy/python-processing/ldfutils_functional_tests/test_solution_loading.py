import ldfutils.connection
#import matplotlib.pyplot as plt


def test_solutions_loading(username, password):

    conn, cursor = ldfutils.connection.create_connection(
        username=username,
        password=password,
        hostname="diogen",
        database="lightning_detection_system"
    )

    lightnings = ldfutils.dbloader.load_solutions(
        cursor,
        ldfutils.dbloader.ConditionGenerator.round_time_interval("2016-06-22 15:00:00", "2016-06-22 15:30:00")
    )

    return len(lightnings) != 0


def test_load_strike(username, password):

    conn, cursor = ldfutils.connection.create_connection(
        username=username,
        password=password,
        hostname="diogen",
        database="lightning_detection_system"
    )

    strikes = ldfutils.dbloader.load_strikes(
        cursor,
        "id=10100"
    )

    #plt.plot(strikes[0].mn_field)
    #plt.show()

    return len(strikes) != 0


def test_full_data_loading(username, password):

    conn, cursor = ldfutils.connection.create_connection(
        username=username,
        password=password,
        hostname="diogen",
        database="lightning_detection_system"
    )

    full_sols = ldfutils.dbloader.load_full_solutions(
        cursor,
        ldfutils.dbloader.ConditionGenerator.round_time_interval("2016-06-22 15:00:00", "2016-06-22 15:30:00")
    )

    return len(full_sols) != 0
