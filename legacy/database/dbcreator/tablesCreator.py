from mysqlutils import *


class TablesCreator:
    table_structure = {
        "experiments": """ (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `description` text NOT NULL,
              PRIMARY KEY (`id`)
        ) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8
        """,
        "devices": """ (
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `name` varchar(200) NOT NULL,
          `active` int(11),
          `lat` double,
          `lon` double,
          PRIMARY KEY (`id`)
        ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8
        """,
        "strikes_dfclient": """ (
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `device_id` int(11) NOT NULL,
          `experiment_id` int(11) NOT NULL,
          `when` datetime NOT NULL,
          `osc_freq` int(11) NOT NULL,
          `count_osc` int(11) NOT NULL,
          `lon` double NOT NULL,
          `lat` double NOT NULL,
          `lon_ew` enum('E','W') NOT NULL,
          `lat_ns` enum('N','S') NOT NULL,
          `E_field` blob NOT NULL,
          `MN_field` blob NOT NULL,
          `MW_field` blob NOT NULL,
          PRIMARY KEY (`id`),
          KEY `experiment_id` (`experiment_id`),
          KEY `device_id` (`device_id`)
        ) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
        """,
        "unified_strikes": """ (
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
          PRIMARY KEY (`id`),
          KEY `experiment_id` (`experiment_id`),
          KEY `device_id` (`device_id`),
          KEY `round_time` (`round_time`),
          KEY `fraction_time` (`fraction_time`)
        ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=9
        """,
        "time_clusters": """ (
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `round_time` datetime NOT NULL,
          `fraction_time` double NOT NULL,
          `strikes_count` int(11) NOT NULL,
          `strikes` text NOT NULL,
          PRIMARY KEY (`id`)
        ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
        """,
        "solutions": """ (
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `round_time` datetime NOT NULL,
          `fraction_time` double NOT NULL,
          `lon` double NOT NULL,
          `lat` double NOT NULL,
          `time_cluster` int(11) NOT NULL,
          `status` TINYINT(1),
          `count_of_detections` int(11) NOT NULL,
          `dispersion` double,
          PRIMARY KEY (`id`),
          KEY `round_time` (`round_time`),
          KEY `fraction_time` (`fraction_time`),
          KEY `status` (`status`),
          KEY `count_of_detections` (`count_of_detections`)
        ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
        """,
        "solutions_energy": """ (
           `id` int(11) NOT NULL AUTO_INCREMENT,
           `solution_id` int(11) NOT NULL,
           `energy` double NOT NULL,
           `energy_sd` double NOT NULL,
           `minimal_distance` double NOT NULL,
           PRIMARY KEY (`id`),
           KEY `solution_id` (`solution_id`),
           KEY `energy` (`energy`),
           KEY `energy_sd` (`energy_sd`),
           KEY `minimal_distance` (`minimal_distance`)
        ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1
        """,
        "strikes_stats": """ (
          `id` int(11) NOT NULL AUTO_INCREMENT,
          `round_time` datetime NOT NULL,
          -- `detections_count` int(11) NOT NULL,
          `solutions_count` int(11) NOT NULL,
          PRIMARY KEY (`id`),
          KEY `round_time` (`round_time`),
          KEY `solutions_count` (`solutions_count`)
        ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
        """,
        "strikes_server": """ (
          `global_id` int(11) NOT NULL AUTO_INCREMENT,
          `id` int(11) NOT NULL,
          `device_id` int(11) NOT NULL,
          `experiment_id` int(11) NOT NULL,
          `when` datetime NOT NULL,
          `osc_freq` int(11) NOT NULL,
          `count_osc` int(11) NOT NULL,
          `when_date` date,
          `lon` double NOT NULL,
          `lat` double NOT NULL,
          `lon_ew` enum('E','W') NOT NULL,
          `lat_ns` enum('N','S') NOT NULL,
          `E_field` blob NOT NULL,
          `MN_field` blob NOT NULL,
          `MW_field` blob NOT NULL,
          PRIMARY KEY (`global_id`),
          KEY `experiment_id` (`experiment_id`),
          KEY `device_id` (`device_id`),
          KEY `when` (`when`),
          KEY `count_osc` (`count_osc`)
        ) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8
        """
    }

    # This is for bad case when two different tables have the same name
    table_names_fix_table = {"strikes_dfclient": "strikes", "strikes_server": "strikes"}

    def fix_name(self, table_name):
        if table_name in self.table_names_fix_table:
            return self.table_names_fix_table[table_name]
        else:
            return table_name

    def create(self, conn_pars, database, table):
        if not (table in self.table_structure):
            raise ValueError("Invalid table name")
        # We need call fix_name() to change unique name to real table
        full_table_name = database + ".`" + self.fix_name(table) + "`"
        cnx, cursor = conn_pars.get_connection_and_cursor()
        print("Re-creating table " + full_table_name + "...")
        cursor.execute("DROP TABLE IF EXISTS " + full_table_name)
        cursor.execute("CREATE TABLE " + full_table_name + self.table_structure[table])
        cnx.commit()
        cursor.close()
        cnx.close()


