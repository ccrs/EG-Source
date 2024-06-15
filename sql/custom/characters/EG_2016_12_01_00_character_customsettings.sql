DROP TABLE IF EXISTS `character_extended`;
CREATE TABLE `character_extended` (
  `guid` int(10) UNSIGNED NOT NULL,
  `customFlags` text,
  PRIMARY KEY (`guid`)
) ENGINE=INNODB DEFAULT CHARSET=utf8
