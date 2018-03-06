--
-- WWLLN solutions table
--

DROP TABLE IF EXISTS `wwlln_solutions`;

CREATE TABLE `wwlln_solutions` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `round_time` datetime NOT NULL,
  `fraction_time` double NOT NULL,
  `lon` double NOT NULL,
  `lat` double NOT NULL,
  `parameter1` double,
  `parameter2` double,
  PRIMARY KEY (`id`),
  KEY `lon` (`lon`),
  KEY `lat` (`lat`),
  KEY `round_time` (`round_time`),
  KEY `fraction_time` (`fraction_time`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=9 ;

