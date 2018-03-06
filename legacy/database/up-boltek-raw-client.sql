
--
-- Table structure for table `strikes`. This is client variant. TO BE RENAIMED!
--

-- @todo Add script to set up server variant of `strikes` table

CREATE TABLE `strikes` (
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
