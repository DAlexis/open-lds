--
-- Table structure for table `unified_strikes`
--

CREATE TABLE `unified_strikes` (
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
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=9 ;

--
-- Table structure for table `time_clusters`
--

CREATE TABLE `time_clusters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `round_time` datetime NOT NULL,
  `fraction_time` double NOT NULL,
  `strikes` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

--
-- Table structure for table `time_clusters`
--

CREATE TABLE `solutions` (
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
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;

--
-- Table structure for table `strikes_stats`
--

CREATE TABLE `strikes_stats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `round_time` datetime NOT NULL,
  -- `detections_count` int(11) NOT NULL,
  `solutions_count` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `round_time` (`round_time`),
  KEY `solutions_count` (`solutions_count`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5 ;
