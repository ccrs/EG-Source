--
DROP TABLE IF EXISTS `character_arena_1v1`;
CREATE TABLE `character_arena_1v1` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Player guidLow',
  `rating` smallint(5) unsigned NOT NULL DEFAULT 0,
  `matchMakerRating` smallint(5) unsigned NOT NULL DEFAULT 1500,
  `weekGames` smallint(5) unsigned NOT NULL DEFAULT 0,
  `weekWins` smallint(5) unsigned NOT NULL DEFAULT 0,
  `seasonGames` smallint(5) unsigned NOT NULL DEFAULT 0,
  `seasonWins` smallint(5) unsigned NOT NULL DEFAULT 0,
  `previousOpponent` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Last opponent guidLow for queue anti-rematch',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
