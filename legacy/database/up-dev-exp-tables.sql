--
-- Table stricture for `devices` table
--

-- @todo Client variant of device and experiments table should have field id_in_server_table

CREATE TABLE `devices` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `active` int(11),
  `lat` double,
  `lon` double,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

--
-- Table structure for table `experiments`
--

CREATE TABLE `experiments` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;


